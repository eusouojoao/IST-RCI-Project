#include "TCP.h"
#include "../common/retry.h"
#include "../error_handling/error_messages.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define MAXREQUESTS 99
#define BUFFER_SIZE 256

/**
 * @brief Sends a message over a TCP connection and reads the response.
 *
 * This function writes a message to the specified file descriptor and then
 * reads the response from the connection. The received message is returned
 * as a newly allocated string.
 *
 * @param fd: the file descriptor of the TCP connection.
 * @param msg_to_send: the message to send.
 *
 * @return a pointer to the received message, or NULL on error.
 */
char *send_message_TCP(int fd, char *msg_to_send) {
  char buffer[BUFFER_SIZE] = {'\0'};

  ssize_t n = write(fd, msg_to_send, strlen(msg_to_send) + 1);
  if (n == -1) {
    system_error("write() failed");
    close(fd);
    return NULL;
  }

  n = recv(fd, buffer, sizeof(buffer), 0);
  if (n == -1) {
    system_error("recv() failed");
    close(fd);
    return NULL;
  }

  char *received_msg = calloc((size_t)n, sizeof(char));
  if (received_msg == NULL) {
    close(fd);
    exit(1);
  }

  memcpy(received_msg, buffer, (size_t)n);
  return received_msg;
}

/**
 * @brief Establishes a connection to the external node and sends a message.
 *
 * This function creates a new TCP connection to the external node specified
 * by the host structure. It then sends the given message to the external node
 * and returns the received response.
 *
 * @param host: a pointer to the host structure containing information about the external
 * node.
 * @param msg_to_send: the message to send to the external node.
 *
 * @return a pointer to the received message, or NULL on error.
 */
char *fetch_bck(host *host, char *msg_to_send) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    system_error("socket() failed");
    return NULL;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, host->ext->IP, &(addr.sin_addr)) != 1) {
    system_error("inet_pton() failed");
    return NULL;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)host->ext->TCP);

  ssize_t n = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (n == -1) {
    system_error("connect() failed");
    close(fd);
    return NULL;
  }

  host->ext->fd = fd;
  return send_message_TCP(host->ext->fd, msg_to_send);
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
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    exit(EXIT_FAILURE);
  }

  // Set socket options for address reuse
  int optval = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    system_error("setsockopt() failed");
    close(fd);
    return -1;
  }

  // Set socket timeouts
  if (set_timeouts(fd) == -1) {
    close(fd);
    return -1;
  }

  // Initialize the socket address structure
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->IP, &(addr.sin_addr)) != 1) {
    system_error("inet_pton() failed");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Set the address family and port number
  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->TCP);

  // Bind the socket to the specified address
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    system_error("bind() failed");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(fd, MAXREQUESTS) < 0) {
    system_error("listen() failed");
    close(fd);
    exit(EXIT_FAILURE);
  }

  return fd;
}
