#include "../hdr/UDP_utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 128

char *send_user_message_UDP(user_args *uip, char *msg) {
  int fd = -1;
  ssize_t n = 0;
  char buffer[BUFFER_SIZE] = "";
  char *received_msg = NULL;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->regIP, &(addr.sin_addr)) != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->regUDP);

  n = sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof(addr));
  if (n == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  /*! TODO: Tratamento de timeout, caso a mensagem se perca, ou o servidor não
   * responda */

  socklen_t addrlen = sizeof(addr);
  n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr,
               &addrlen);
  if (n == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  received_msg = calloc((size_t)n + 1, sizeof(char));
  strncpy(received_msg, buffer, (size_t)n + 1);

  close(fd);
  return received_msg;
}
