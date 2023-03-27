#include "TCP.h"
#include "../../error_handling/error_messages.h"
#include "core_common.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXREQUESTS 99
#define BUFFER_SIZE 256

/**
 * @brief Sends a message over a TCP socket.
 *
 * This function sends a message over a TCP socket using the provided file
 * descriptor. It ensures that the entire message is sent by repeatedly calling
 * write() until all bytes are sent or an error occurs.
 *
 * @param fd: the file descriptor of the socket to send the message on
 * @param msg_to_send: the message to be sent
 * @param msglen: the length of the message to be sent
 *
 * @return the total number of bytes sent or -1 if an error occurred.
 */
ssize_t write_msg_TCP(int fd, char *msg_to_send, size_t msglen) {
  ssize_t total_sent = 0, bytes_sent = 0;

  // loop until the entire message is sent
  while (total_sent < (ssize_t)msglen) {
    bytes_sent = write(fd, msg_to_send + total_sent, msglen - (size_t)total_sent);
    if (bytes_sent == -1) {
      return -1;
    }

    total_sent += bytes_sent;
  }

  return total_sent;
}

/**
 * @brief Establishes a connection to the external node and sends a message.
 *
 * This function creates a new TCP connection to the external node specified
 * by the host structure. It then sends the given message to the external node.
 *
 * @param host: a pointer to the host structure containing information about the external
 * node.
 * @param msg_to_send: the message to send to the external node.
 *
 * @return
 */
ssize_t fetch_bck(host *host, char *msg_to_send) {
  // Create a new TCP socket
  int fd = create_socket(TCP);
  if (fd == -1) {
    return -1;
  }

  // Initialize the socket address structure
  struct sockaddr_in addr;
  if (setup_sockaddr_in(&addr, host->ext->IP, host->ext->TCP) == -1) {
    close(fd);
    return -1;
  }

  ssize_t n = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (n == -1) {
    system_error("connect() failed");
    close(fd);
    return -1;
  }

  host->ext->fd = fd;
  return write_msg_TCP(host->ext->fd, msg_to_send, strlen(msg_to_send));
}

/**
 * @brief Creates a TCP listening socket using the user-defined arguments.
 *
 * This function creates a socket, sets socket options for address reuse and timeouts,
 * initializes the socket address structure, binds the socket to the address, and
 * listens for incoming connections.
 *
 * @param uip: pointer to a user_args structure containing IP and port information
 *
 * @return the file descriptor of the created listening socket, or -1 on error.
 */
int create_listen_socket(user_args *uip) {
  // Create a new TCP socket
  int fd = create_socket(TCP);
  if (fd == -1) {
    return -1;
  }

  // Set socket options for address reuse
  int optval = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    system_error("setsockopt() failed");
    close(fd);
    return -1;
  }

  // Initialize the socket address structure
  struct sockaddr_in addr;
  if (setup_sockaddr_in(&addr, uip->IP, uip->TCP) == -1) {
    close(fd);
    return -1;
  }

  // Bind the socket to the specified address
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    system_error("bind() failed");
    close(fd);
    return -1;
  }

  // Listen for incoming connections
  if (listen(fd, MAXREQUESTS) < 0) {
    system_error("listen() failed");
    close(fd);
    return -1;
  }

  return fd;
}
