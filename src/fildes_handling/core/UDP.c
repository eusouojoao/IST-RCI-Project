#include "UDP.h"
#include "../../common/retry.h"
#include "../../error_handling/error_messages.h"
#include "core_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/** @brief Sends a message over a UDP connection.
 *
 * This function sends the given message to the specified destination IP and
 * port using a given UDP socket file descriptor.
 *
 * @param fd: socket file descriptor
 * @param addr: pointer to the sockaddr_in structure containing the destination IP and
 * port
 * @param msg: message to send
 *
 * @return The number of bytes sent, or -1 on error.
 */
ssize_t send_msg_UDP(int fd, struct sockaddr_in *addr, char *msg) {
  ssize_t n;
  RETRY(sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)addr, sizeof(*addr)),
        MAX_ATTEMPTS, n);

  return n;
}

/** @brief Receives a message over a UDP connection.
 *
 * This function reads a message from a UDP connection using a given socket
 * file descriptor and stores it in the provided buffer.
 *
 * @param fd: socket file descriptor
 * @param addr: pointer to the sockaddr_in structure to store the sender's address
 * @param buffer: buffer to store the received message
 * @param buffer_size: size of the buffer
 *
 * @return the number of bytes received, or -1 on error.
 */
ssize_t recv_msg_UDP(int fd, struct sockaddr_in *addr, char *buffer, size_t buffer_size) {
  ssize_t n;
  socklen_t addrlen = sizeof(*addr);
  RETRY(recvfrom(fd, buffer, buffer_size, 0, (struct sockaddr *)addr, &addrlen),
        MAX_ATTEMPTS, n);

  return n;
}

/** @brief Sends a message over a UDP connection and reads the response.
 *
 * This function creates a new UDP socket, sets the appropriate timeouts,
 * and sends the given message to the specified destination IP and port.
 * It then reads the response from the connection and returns it as
 * a newly allocated string.
 *
 * @param uip: pointer to the user_args structure containing the destination IP and
 * port
 * @param msg: message to send
 *
 * @return A pointer to the received message, or NULL on error.
 */
char *send_and_receive_msg_UDP(user_args *uip, char *msg) {
  char buffer[BUFFER_SIZE] = {'\0'};
  ssize_t n = 0;

  int fd = create_socket(UDP);
  if (fd == -1) {
    return NULL;
  }

  if (set_timeouts(fd) == -1) {
    return NULL;
  }

  struct sockaddr_in addr;
  if (setup_sockaddr_in(&addr, uip->regIP, uip->regUDP) == -1) {
    close(fd);
    return NULL;
  }

  n = send_msg_UDP(fd, &addr, msg);
  if (n == -1) {
    system_error("sendto() failed");
    close(fd);
    return NULL;
  }

  n = recv_msg_UDP(fd, &addr, buffer, sizeof(buffer));
  if (n == -1) {
    system_error("recvfrom() failed");
    close(fd);
    return NULL;
  }

  char *received_msg = calloc((size_t)n + 1, sizeof(char));
  if (received_msg == NULL) {
    close(fd);
    system_error("calloc() failed");
    exit(EXIT_FAILURE);
  }

  close(fd);
  memcpy(received_msg, buffer, (size_t)n);
  return received_msg;
}
