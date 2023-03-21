#include "descriptor_control.h"
#include "../error_handling/error_messages.h"
#include "process_descriptors.h"
#include "socket_protocols_interface/delete_node_module.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
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
 * @param working_set[in]: pointer to a file descriptor set to wait for events
 * on. On return, this set will contain only the descriptors that became ready during
 * the wait.
 * @param host[in]: pointer to a host representing the system to wait on.
 * @param timeout[in]: pointer to a timeval struct specifying the maximum time to
 * wait for events. If this value is NULL, the function will block indefinitely.
 * @param counter[out]: pointer to an integer that will receive the number of
 * descriptors that became ready during the wait.
 *
 * @return (int) 0 on success, -1 on failure.
 */
int wait_for_ready_fildes(host *host, fd_set *working_set, int *counter,
                          struct timeval *timeout) {

  do {
    *counter = select(get_maxfd(host) + 1, working_set, (fd_set *)NULL, (fd_set *)NULL,
                      (struct timeval *)timeout);
  } while (*counter == -1 && errno == EINTR); // Ignore SIGQUIT

  if (*counter <= 0) {
    system_error("In wait_for_ready_fildes() -> select() failed");
    return -1;
  }

  return 0;
}

/**
 * @brief Manages file descriptors for keyboard input, new connections, and neighbor
 * nodes in a distributed network.
 *
 * This function iterates through the file descriptors of interest and handles the
 * following cases:
 * 1. Keyboard input: processes user input using the handle_keyboard_input()
 * function.
 * 2. New connections: processes new incoming connections using the
 * handle_new_connection() function.
 * 3. Neighbor nodes: processes messages from neighbor nodes using the
 * handle_neighbour_nodes() function.
 *
 * The function stops when the counter reaches zero or an error occurs during
 * processing.
 *
 * @param[in] host: pointer to the host structure.
 * @param[in] working_set: pointer to a file descriptor set containing the
 * descriptors to monitor.
 * @param[in] counter: pointer to the counter of file descriptors to process.
 *
 * @return (int) Returns 1 if the function processes all file descriptors
 * successfully, -1 if an error occurs during processing, or 0 if the application
 * should exit.
 */
int fildes_control(host *host, fd_set *working_set, int *counter) {
  while ((*counter)-- > 0) {
    // Handle keyboard input
    if (FD_ISSET(STDIN_FILENO, working_set)) {
      FD_CLR(STDIN_FILENO, working_set);
      if (handle_keyboard_input(host) == 0) {
        return 0;
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
  }

  return 1; // OK
}

/**
 * @brief Handles the stdin's file descriptor.
 *
 * This function reads data from the standard input (keyboard) into a buffer and
 * processes the data using the process_keyboard_input() function. In case of any
 * error during reading, the function handles the error appropriately.
 *
 * @param[in] host: pointer to the host structure.
 *
 * @return (int) Returns the result of process_keyboard_input() if the function
 * processes the keyboard input successfully, -1 if an error occurs during reading
 * data.
 */
int handle_keyboard_input(host *host) {
  char *buffer = calloc(SIZE + 1, sizeof(char));
  if (buffer == NULL) {
    system_error("calloc() failed");
    return -1;
  }

  ssize_t bytes_read = read(STDIN_FILENO, buffer, SIZE);
  if (bytes_read <= 0) {
    system_error("read() failed");
    free(buffer);
    return -1;
  }

  // Process standard input - keyboard
  int r = process_keyboard_input(host, buffer);
  free(buffer);
  return r;
}

/**
 * @brief Accepts and processes new incoming connections in a distributed network.
 *
 * This function accepts a new connection from the listening file descriptor, reads
 * data from the newly connected node into a buffer, and processes the data using the
 * process_new_connection() function. In case of any error or if the connection is
 * closed, the function handles these cases appropriately.
 *
 * @param[in] host: pointer to the host structure containing the listening file
 * descriptor.
 *
 * @return (int) Returns 1 if the function processes the new connection successfully,
 * -1 if an error occurs during accepting a connection or reading data.
 */
int handle_new_connection(host *host) {
  struct sockaddr in_addr;
  socklen_t in_addrlen = sizeof(in_addr);

  int new_fd = accept(host->listen_fd, &in_addr, &in_addrlen);
  if (new_fd == -1) {
    system_error("accept() failed");
    return -1;
  }

  char *buffer = calloc(SIZE + 1, sizeof(char));
  if (buffer == NULL) {
    system_error("calloc() failed");
    close(new_fd);
    return -1;
  }

  ssize_t bytes_read = read(new_fd, buffer, SIZE);
  if (bytes_read == 0) {
    close(new_fd);
    free(buffer);
    return 1;
  } else if (bytes_read < 0) {
    system_error("read() failed");
    close(new_fd);
    free(buffer);
    return -1;
  }

  /* Process the new accepted file descriptor */
  process_new_connection(host, new_fd, buffer);
  free(buffer);
  return 1;
}

/**
 * @brief Handles messages from neighbor nodes in a distributed network.
 *
 * This function iterates through the list of neighbor nodes, checks if there is any
 * data available to read from the file descriptor of a node, reads the data into a
 * buffer, and processes the messages in the buffer. If a node leaves the network or
 * an error occurs during reading, the function handles these cases appropriately.
 *
 * @param[in] host: pointer to the host structure containing the node list.
 * @param[in] working_set: pointer to a file descriptor set containing the
 * descriptors to monitor.
 *
 * @return (int) Returns 1 if the function processes all messages successfully, -1 if
 * an error occurs during memory allocation or reading data.
 */
int handle_neighbour_nodes(host *host, fd_set *working_set) {
  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    if (FD_ISSET(temp->fd, working_set)) {
      FD_CLR(temp->fd, working_set);

      char *buffer = calloc((SIZE << 4) + 1, sizeof(char));
      if (buffer == NULL) {
        system_error("calloc() failed");
        return -1;
      }

      ssize_t bytes_read = read(temp->fd, buffer, SIZE << 4);
      if (bytes_read == 0) {
        // Node left the network
        delete_node(host, temp->fd);
        free(buffer);
        break;
      } else if (bytes_read < 0) {
        system_error("read() failed");
        free(buffer);
        return -1;
      }

      // Process each message in the buffer individually
      char *token = strtok(buffer, "\n");
      while (token != NULL) {
        process_neighbour_nodes(host, temp, token);
        token = strtok(NULL, "\n");
      }

      free(buffer);
      break;
    }
  }

  return 1;
}
