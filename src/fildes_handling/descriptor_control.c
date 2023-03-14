#include "descriptor_control.h"
#include "../error_handling/error_messages.h"
#include "process_descriptors.h"
#include "socket_protocols_interface/delete_node_module.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SIZE 256

/**
 * @brief This function updates a given fd_set with file descriptors that need to be
 * monitored for input events. It first clears the working_set using FD_ZERO()
 * to ensure that it's empty. Then it adds the file descriptor for standard input
 * stream (STDIN_FILENO) and the listening socket file descriptor (host->listen_fd)
 * to the working_set using FD_SET(). After that, it iterates over a linked list of
 * nodes in the host structure and adds their file descriptors to the working_set
 * using FD_SET().
 *
 * @param host: a pointer to a host structure that contains the node list and
 * listening socket file descriptor
 * @param working_set: a pointer to a fd_set structure that will be updated with file
 * descriptors to be monitored for input or output events
 */
void update_working_set(host *host, fd_set *working_set) {
  FD_ZERO(working_set);
  FD_SET(STDIN_FILENO, working_set);

  FD_SET(host->listen_fd, working_set);
  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    FD_SET(temp->fd, working_set);
  }
}

/**
 * @brief Returns the maximum file descriptor in the host's node list
 *
 * The function returns the maximum file descriptor from the list of nodes contained
 * in the host structure. If the list of nodes is empty, the function returns the
 * host's listen file descriptor.
 *
 * @param host: the host structure containing the list of nodes to search.
 * @return The maximum file descriptor in the host's structure
 */
int get_maxfd(host *host) {
  return host->node_list == NULL ? host->listen_fd : host->node_list->fd;
}

/**
 * @brief Wait for file descriptors in a set to become ready for I/O.
 *
 * This function blocks until at least one of the file descriptors in the given set
 * becomes ready for I/O, or until the specified timeout expires. It uses the
 * select() system call to wait for events on the file descriptors.
 *
 * @param working_set: pointer to a file descriptor set to wait for events
 * on. On return, this set will contain only the descriptors that became ready during
 * the wait.
 * @param host: pointer to a host representing the system to wait on.
 * @param timeout: pointer to a timeval struct specifying the maximum time to
 * wait for events. If this value is NULL, the function will block indefinitely.
 * @param counter: pointer to an integer that will receive the number of descriptors
 * that became ready during the wait.
 *
 * @return 0 on success, -1 on failure.
 */
int wait_for_ready_fildes(host *host, fd_set *working_set, int *counter,
                          struct timeval *timeout) {

  *counter = select(get_maxfd(host) + 1, working_set, (fd_set *)NULL, (fd_set *)NULL,
                    (struct timeval *)timeout);
  if (*counter <= 0) {
    system_error("In wait_for_ready_fildes() -> select() failed");
    return -1;
  }

  return 0;
}

/**
 * @brief Controls the file descriptors and processes input from different sources,
 * including standard input and sockets from neighboring nodes.
 *
 * @note This function checks if there is data to be read from standard input or any
 * of the sockets of the neighboring nodes. If there is data to be read, it
 * processes the input accordingly, calling the corresponding functions to handle
 * it.
 *
 * @param working_set: set of file descriptors to wait for events
 * @param host: pointer to the host struct
 *
 * @return 0 on success, -1 on failure
 */
int fildes_control(host *host, fd_set *working_set, int *counter) {
  while (*counter > 0) {
    // Handle keyboard input
    if (FD_ISSET(STDIN_FILENO, working_set)) {
      FD_CLR(STDIN_FILENO, working_set);
      if (handle_keyboard_input(host) == -1) {
        return -1;
      }
      continue;
    }

    // Handle new connections
    if (FD_ISSET(host->listen_fd, working_set)) {
      FD_CLR(host->listen_fd, working_set);
      if (handle_new_connection(host) == -1) {
        return -1;
      }
      continue;
    }

    // Handle communication with neighbour nodes
    if (handle_neighbour_nodes(host, working_set) == -1) {
      return -1;
    }

    (*counter)--;
  }

  return 0;
}

int handle_keyboard_input(host *host) {
  char *buffer = calloc(SIZE, sizeof(char));
  if (buffer == NULL) {
    system_error("In process_keyboard_input() -> calloc() failed");
    return -1;
  }

  ssize_t bytes_read = read(STDIN_FILENO, buffer, SIZE);
  if (bytes_read <= 0) {
    system_error("In process_keyboard_input() -> read() failed");
    free(buffer);
    return -1;
  }

  /* Process standard input - keyboard */
  process_keyboard_input(host, buffer);
  free(buffer);
  return 0;
}

int handle_new_connection(host *host) {
  struct sockaddr in_addr;
  socklen_t in_addrlen = sizeof(in_addr);

  int new_fd = accept(host->listen_fd, &in_addr, &in_addrlen);
  if (new_fd == -1) {
    system_error("In process_new_connection() -> accept() failed");
    return -1;
  }

  char *buffer = calloc(SIZE, sizeof(char));
  if (buffer == NULL) {
    system_error("In process_new_connection() -> calloc() failed");
    close(new_fd);
    return -1;
  }

  ssize_t bytes_read = read(new_fd, buffer, SIZE);
  if (bytes_read == 0) {
    close(new_fd);
    free(buffer);
    return 0;
  } else if (bytes_read < 0) {
    system_error("In process_new_connection() -> read() failed");
    close(new_fd);
    free(buffer);
    return -1;
  }

  /* Process the new accepted file descriptor */
  process_new_connection(host, new_fd, buffer);
  free(buffer);
  return 0;
}

int handle_neighbour_nodes(host *host, fd_set *working_set) {
  node *temp = host->node_list;
  while (temp != NULL) {
    if (FD_ISSET(temp->fd, working_set)) {
      FD_CLR(temp->fd, working_set);

      char *buffer = calloc(SIZE << 4, sizeof(char));
      if (buffer == NULL) {
        system_error("In process_neighbour_nodes() -> calloc() failed");
        return -1;
      }

      ssize_t bytes_read = read(temp->fd, buffer, SIZE);
      if (bytes_read == 0) {
        /* Node left the network */
        delete_node(host, temp->fd);
        free(buffer);
        break;
      } else if (bytes_read < 0) {
        system_error("In process_neighbour_nodes() -> read() failed");
        free(buffer);
        return -1;
      }

      char *token = strtok(buffer, "\n");
      while (token != NULL) {
        /* Process external and */
        process_neighbour_nodes(host, temp, token);
        token = strtok(NULL, "\n");
      }

      free(buffer);
      break;
    }
  }

  return 0;
}
