#include "UDP.h"
#include "../../common/retry.h"
#include "../../error_handling/error_messages.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

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
char *send_message_UDP(user_args *uip, char *msg) {
  ssize_t n = 0;
  char buffer[BUFFER_SIZE] = {'\0'};

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    system_error("socket() failed");
    return NULL;
  }

  if (set_timeouts(fd) == -1) {
    return NULL;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->regIP, &(addr.sin_addr)) != 1) {
    system_error("inet_pton() failed");
    close(fd);
    return NULL;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->regUDP);

  RETRY( // Will retry if it fails...
      sendto(fd, msg, strlen(msg) + 1, 0, (struct sockaddr *)&addr, sizeof(addr)),
      MAX_ATTEMPTS, n /* return value */);

  if (n == -1) {
    system_error("sendto() failed");
    close(fd);
    return NULL;
  }

  socklen_t addrlen = sizeof(addr);
  RETRY( // Will retry if it fails...
      recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen),
      MAX_ATTEMPTS, n /* return value */);

  if (n == -1) {
    system_error("recvfrom() failed");
    close(fd);
    return NULL;
  }

  char *received_msg = calloc((size_t)n + 1, sizeof(char));
  if (received_msg == NULL) {
    close(fd);
    exit(1);
  }

  close(fd);
  memcpy(received_msg, buffer, (size_t)n);
  return received_msg;
}
