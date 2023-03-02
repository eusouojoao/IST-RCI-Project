#include "../hdr/TCP_utils.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define MAXREQUESTS 99

int create_listen_socket(user_args *uip) {
  int fd = -1;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->IP, &(addr.sin_addr)) != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->TCP);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    /*error*/ exit(EXIT_FAILURE);
  }

  if (listen(fd, MAXREQUESTS) < 0) {
    /*error*/ exit(EXIT_FAILURE);
  }

  return fd;
}
