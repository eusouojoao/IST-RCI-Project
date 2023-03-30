#include "descriptor_control.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/circular_buffer.h"
#include "../../essentials/new_connections_list.h"
#include "../../misc/prompts.h"
#include "../core/TCP.h"
#include "../custom_protocols_interface/delete_node_module.h"
#include "process_descriptors.h"

#include <errno.h>
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
 * using FD_SET(), it acts similarly to the new_connection's list.
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

  for (new_connection *nc = host->new_connections_list; nc != NULL; nc = nc->next) {
    FD_SET(nc->new_fd, working_set);
  }

  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    FD_SET(temp->fd, working_set);
  }
}

/**
 * @brief Helper function that returns the maximum file descriptor
 *        from the new_connections list.
 *
 * @param nc: pointer to the head of the new_connections list
 * @return the maximum file descriptor in the new_connections list.
 */
static int max_fd_from_new_connections(new_connection *nc) {
  int max_fd = -1;
  while (nc != NULL) {
    if (nc->new_fd > max_fd) {
      max_fd = nc->new_fd;
    }
    nc = nc->next;
  }
  return max_fd;
}

/**
 * @brief Helper function that returns the maximum file descriptor
 *        from the nodes list.
 *
 * @param n: pointer to the head of the nodes list
 * @return the maximum file descriptor in the nodes list.
 */
static int max_fd_from_nodes(node *n) {
  int max_fd = -1;
  while (n != NULL) {
    if (n->fd > max_fd) {
      max_fd = n->fd;
    }
    n = n->next;
  }
  return max_fd;
}

/**
 * @brief Returns the maximum file descriptor.
 *
 * The function returns the maximum file descriptor from the host's structure.
 *
 * @param host: the host structure containing the list of nodes to search
 * @return the maximum file descriptor in the host's structure.
 */
int get_maxfd(host *host) {
  int max_fd = host->listen_fd;
  int nc_max_fd = max_fd_from_new_connections(host->new_connections_list);
  int node_max_fd = max_fd_from_nodes(host->node_list);

  if (nc_max_fd > max_fd) {
    max_fd = nc_max_fd;
  }
  if (node_max_fd > max_fd) {
    max_fd = node_max_fd;
  }

  return max_fd;
}

/**
 * @brief Wait for file descriptors in a set to become ready for I/O.
 *
 * This function blocks until at least one of the file descriptors in the given set becomes
 * ready for I/O, or until the specified timeout expires. It uses the select() system call to
 * wait for events on the file descriptors.
 *
 * @param working_set: pointer to a file descriptor set to wait for events on. On return, this
 * set will contain only the descriptors that became ready during the wait.
 * @param host: pointer to a host representing the system to wait on.
 * @param timeout: pointer to a timeval struct specifying the maximum time to
 * wait for events. If this value is NULL, the function will block indefinitely.
 * @param counter[out]: pointer to an integer that will receive the number of
 * descriptors that became ready during the wait.
 *
 * @return 1 on success, 0 if a timeout occurs and -1 on failure.
 */
int wait_for_ready_fildes(host *host, fd_set *working_set, int *counter,
                          struct timeval *timeout) {

  do {
    // Local copy of the timeout
    struct timeval local_timeout = {
        .tv_sec = timeout->tv_sec,
        .tv_usec = timeout->tv_usec,
    };

    *counter = select(get_maxfd(host) + 1, working_set, (fd_set *)NULL, (fd_set *)NULL,
                      (struct timeval *)&local_timeout);
  } while (*counter == -1 && errno == EINTR); // Ignore SIGQUIT

  if (*counter == 0) {
    return 0;
  }

  if (*counter < 0) {
    system_error("select() failed");
    return -1;
  }

  return 1;
}

/**
 * @brief Manages file descriptors for keyboard input, new connections, and neighbor
 * nodes in a distributed network.
 *
 * This function iterates through the file descriptors of interest and handles the following
 * cases:
 * 1. Keyboard input: processes user input using the handle_keyboard_input() function.
 * 2. New connections: processes new incoming connections using the handle_new_connection()
 * function.
 * 3. Neighbor nodes: processes messages from neighbor nodes using the handle_neighbour_nodes()
 * function.
 *
 * The function stops when the counter reaches zero or an error occurs during
 * processing.
 *
 * @param host: pointer to the host structure.
 * @param working_set: pointer to a file descriptor set containing the descriptors to monitor.
 * @param counter: pointer to the counter of file descriptors to process.
 *
 * @return 1 if the function processes all file descriptors successfully, -1 if an error occurs
 * during processing, or 0 if the application should exit.
 */
int fildes_control(host *host, fd_set *working_set, int *counter, char *buffer) {
  if (!(*counter)) {
    handle_inactive_connections(host);
  }

  while ((*counter)-- > 0) {
    // Handle keyboard input
    if (FD_ISSET(STDIN_FILENO, working_set)) {
      FD_CLR(STDIN_FILENO, working_set);
      if (handle_keyboard_input(host, buffer) == 0) {
        return 0;
      }
      continue;
    }

    // Handle new connections
    if (FD_ISSET(host->listen_fd, working_set)) {
      FD_CLR(host->listen_fd, working_set);
      if (handle_new_connection(host, buffer) == -1) {
        return -1;
      }
      continue;
    }

    // Handle queued new connections
    if (handle_queued_connections(host, working_set, buffer)) {
      continue;
    }

    // Handle communication with neighbour nodes
    if (handle_neighbour_nodes(host, working_set, buffer) == -1) {
      return -1;
    }
  }

  return 1;
}

/**
 * @brief Handles the stdin's file descriptor.
 *
 * This function reads data from the standard input (keyboard) into a buffer and processes the
 * data using the process_keyboard_input() function. In case of any error during reading, the
 * function handles the error appropriately.
 *
 * @param host: pointer to the host structure
 * @param buffer: temporary buffer to read() from the file descriptor
 *
 * @return the result of process_keyboard_input() if the function processes the keyboard input
 * successfully, -1 if an error occurs during reading data.
 */
int handle_keyboard_input(host *host, char *buffer) {
  ssize_t bytes_read = read(STDIN_FILENO, buffer, SIZE - 1);
  if (bytes_read <= 0) {
    system_error("read() failed");
    return -1;
  }
  buffer[bytes_read] = '\0';

  // Process standard input - keyboard
  int r = process_keyboard_input(host, buffer);
  // Print prompt
  prompt();
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
 * @param host: pointer to the host structure containing the listening file
 * descriptor.
 * @param buffer: temporary buffer to read() from the file descriptor
 *
 * @return 1 if the function processes the new connection successfully, -1 if an error occurs
 * during accepting a connection or reading data.
 */
int handle_new_connection(host *host, char *buffer) {
  struct sockaddr in_addr;
  socklen_t in_addrlen = sizeof(in_addr);

  int new_fd = accept(host->listen_fd, &in_addr, &in_addrlen);
  if (new_fd == -1) {
    system_error("accept() failed");
    return -1;
  }

  ssize_t bytes_read = read(new_fd, buffer, SIZE - 1);
  if (bytes_read == 0) {
    close(new_fd);
    return 1;
  } else if (bytes_read < 0) {
    system_error("read() failed");
    close(new_fd);
    return -1;
  }
  buffer[bytes_read] = '\0';

  insert_new_connection(host, new_fd, buffer);

  // Process the new accepted file descriptor
  process_new_connection(host, host->new_connections_list);
  return 1;
}

/**
 * @brief Processes new incoming connections that provided incomplete messages.
 *
 * This function reads data from the newly connected node into a circular buffer, attempting to
 * complete a message, and processes the data using the process_new_connection() function. In
 * case of any error or if the connection is closed, the function handles these cases
 * appropriately.
 *
 * @param host: pointer to the host structure.
 * @param working_set: pointer to a file descriptor set containing the descriptors to monitor.
 * @param buffer: temporary buffer to read() from the file descriptor
 *
 * @return 1 if the function processes the new connection successfully, -1 if an error occurs
 */
int handle_queued_connections(host *host, fd_set *working_set, char *buffer) {
  new_connection *temp = NULL;
  for (temp = host->new_connections_list; temp != NULL; temp = temp->next) {
    if (FD_ISSET(temp->new_fd, working_set)) {
      FD_CLR(temp->new_fd, working_set);

      ssize_t bytes_read = read(temp->new_fd, buffer, SIZE - 1);
      if (bytes_read == 0) {
        close(temp->new_fd);
        remove_new_connection(host, temp->new_fd);
        return 1;
      } else if (bytes_read < 0) {
        system_error("read() failed");
        close(temp->new_fd);
        remove_new_connection(host, temp->new_fd);
        return -1;
      }
      buffer[bytes_read] = '\0';

      size_t len = strlen(buffer);
      if (cb_write(temp->cb, buffer, len) != len) {
        close(temp->new_fd);
        remove_new_connection(host, temp->new_fd);
        return -1;
      }

      process_new_connection(host, host->new_connections_list);
      return 1;
    }
  }

  return 0;
}

/**
 * @brief Clean inactive connections from the new_connections_list.
 *
 * This function iterates through the new_connections_list of the given host
 * and removes connections that have been inactive for more than TIMEOUT_SEC.
 * It also closes the file descriptor associated with the removed connections
 * and frees the memory occupied by the corresponding new_connection structure.
 *
 * @param host: pointer to the host structure
 */
static void clean_inactive_new_connections(host *host) {
  new_connection **current = &host->new_connections_list;

  while (*current != NULL) {
    if ((*current)->time_to_live == 0) {
      // Connection timeout exceeded
      new_connection *to_delete = *current;
      *current = to_delete->next;

      // Close the file descriptor and free the resources
      close(to_delete->new_fd);
      free(to_delete->cb), free(to_delete);
    } else {
      // Move to the next connection
      current = &((*current)->next);
    }
  }
}

/**
 * @brief Decrements the time to live for each new connection in the host's new connections
 * list.
 *
 * This function iterates through the new connections list in the host and decrements the
 * time_to_live for each connection.
 *
 * @param host: pointer to the host structure containing the new connections list.
 */
static void decrement_new_connection_timers(host *host) {
  new_connection *current = host->new_connections_list;
  while (current != NULL) {
    current->time_to_live--;
    current = current->next;
  }
}

/**
 * @brief Handles inactive connections in the host's new connections list.
 *
 * This function first calls decrement_new_connection_timers() to update the time_to_live for
 * each new connection, and then calls clean_inactive_new_connections() to remove any expired
 * connections from the list.
 *
 * @param host: pointer to the host structure containing the new connections list.
 */
void handle_inactive_connections(host *host) {
  decrement_new_connection_timers(host);
  clean_inactive_new_connections(host);
}

/**
 * @brief Handles messages from neighbor nodes in a distributed network.
 *
 * This function iterates through the list of neighbor nodes, checks if there is any
 * data available to read from the file descriptor of a node, reads the data into a
 * buffer, and processes the messages in the buffer. If a node leaves the network or
 * an error occurs during reading, the function handles these cases appropriately.
 *
 * @param host: pointer to the host structure containing the node list.
 * @param working_set: pointer to a file descriptor set containing the descriptors to monitor.
 * @param buffer: temporary buffer to read() from the file descriptor
 *
 * @return 1 if the function processes all messages successfully, -1 if an error occurs
 */
int handle_neighbour_nodes(host *host, fd_set *working_set, char *buffer) {
  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    if (FD_ISSET(temp->fd, working_set)) {
      FD_CLR(temp->fd, working_set);

      ssize_t bytes_read = read(temp->fd, buffer, SIZE - 1);
      if (bytes_read == 0) {
        // Node left the network
        delete_node(host, temp->fd);
        return 1;
      } else if (bytes_read < 0) {
        system_error("read() failed");
        return -1;
      }
      buffer[bytes_read] = '\0';

      size_t len = strlen(buffer);
      if (cb_available(temp->cb) < len) {
        cb_flush(temp->cb);
      }

      // Store the read message in the circular buffer
      if (cb_write(temp->cb, buffer, len) != len) {
        return -1;
      }

      // Process each complete message in the circular buffer individually
      char read_buffer[SIZE] = {'\0'};
      while (cb_read_LF(temp->cb, read_buffer, sizeof(read_buffer) - 1)) {
        process_neighbour_nodes(host, temp, read_buffer);
      }
      break;
    }
  }

  return 1;
}
