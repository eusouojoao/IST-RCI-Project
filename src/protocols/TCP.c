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
 * @brief Sends a message over a TCP socket.
 *
 * This function sends a message over a TCP socket using the provided file
 * descriptor. It ensures that the entire message is sent by repeatedly calling
 * write() until all bytes are sent or an error occurs.
 *
 * @param fd: The file descriptor of the socket to send the message on
 * @param msg_to_send: The message to be sent
 * @param msglen: The length of the message to be sent
 *
 * @return the total number of bytes sent or -1 if an error occurred.
 */
ssize_t send_msg_TCP(int fd, char *msg_to_send, size_t msglen) {
  ssize_t total_bytes_sent = 0;
  ssize_t bytes_sent = 0;
  const char *buffer = msg_to_send;

  // Loop until the entire message is sent
  while (total_bytes_sent < (ssize_t)msglen) {
    bytes_sent = write(fd, buffer + total_bytes_sent, msglen - (size_t)total_bytes_sent);
    if (bytes_sent == -1) {
      return -1;
    }

    total_bytes_sent += bytes_sent;
  }

  return total_bytes_sent;
}

/**
 * @brief Receives a message over a TCP socket.
 *
 * This function receives a message over a TCP socket using the provided file
 * descriptor. It reads data one byte at a time into the provided buffer, stopping
 * when the buffer is full, the connection is closed, or a newline character is
 * encountered.
 *
 * @param fd The file descriptor of the socket to receive the message on
 * @param buffer The buffer to store the received message in
 * @param size The size of the buffer
 *
 * @return the total number of bytes received or -1 if an error occurred.
 */
ssize_t recv_msg_TCP(int fd, char *buffer, size_t size) {
  ssize_t bytes_received = 0;

  while (bytes_received < (ssize_t)size) {
    ssize_t result = read(fd, buffer + bytes_received, 1); // Read one byte at a time
    if (result < 0) {
      return -1;
    } else if (result == 0) {
      // The sender has closed the connection or no more data is available
      break;
    }

    bytes_received += result;

    if (buffer[bytes_received - 1] == '\n') {
      // Newline character encountered, stop reading
      break;
    }
  }

  return bytes_received;
}

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

  ssize_t n = send_msg_TCP(fd, msg_to_send, strlen(msg_to_send));
  if (n == -1) {
    system_error("write() failed");
    close(fd);
    return NULL;
  }

  n = recv_msg_TCP(fd, buffer, sizeof(buffer) - 1);
  if (n == -1) {
    system_error("read() failed");
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
