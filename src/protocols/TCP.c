#include "TCP.h"
#include "../error_handling/error_messages.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define MAXREQUESTS 99
#define BUFFER_SIZE 128

char *fetch_bck(host *host, char *msg) {
  int fd = -1;
  ssize_t n = 0;
  char buffer[BUFFER_SIZE] = "";
  char *received_msg = NULL;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    system_error("In fetch_bck() ->" RED " socket() failed");
    return NULL;
  }

  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, host->ext->IP, &(addr.sin_addr)) != 1) {
    system_error("In fetch_bck() ->" RED " inet_pton() failed");
    return NULL;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)host->ext->TCP);

  /* Inicializar a estrutura timeval para o timeout */
  struct timeval timeout;
  timeout.tv_sec = 15;
  timeout.tv_usec = 0;

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
    system_error("In fetch_bck() ->" RED " setsockopt() failed");
    close(fd);
    return NULL;
  }

  n = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (n == -1) {
    system_error("In fetch_bck() ->" RED " connect() failed");
    close(fd);
    return NULL;
  }

  n = write(fd, msg, sizeof(msg));
  if (n == -1) {
    system_error("In fetch_bck() ->" RED " write() failed");
    close(fd);
    return NULL;
  }

  n = read(fd, buffer, sizeof(buffer));
  if (n == -1) {
    system_error("In fetch_bck() ->" RED " read() failed");
    close(fd);
    return NULL;
  }

  received_msg = calloc((size_t)n + 1, sizeof(char));
  strncpy(received_msg, buffer, (size_t)n + 1);

  host->ext->fd = fd;
  return received_msg;
}

int create_listen_socket(user_args *uip) {
  int fd = -1;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->IP, &(addr.sin_addr)) != 1) {
    close(fd);
    /*error*/ exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->TCP);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    system_error("In create_listen_socket() ->" RED " bind() failed");
    close(fd);
    /*error*/ exit(EXIT_FAILURE);
  }

  if (listen(fd, MAXREQUESTS) < 0) {
    system_error("In create_listen_socket() ->" RED " listen() failed");
    close(fd);
    /*error*/ exit(EXIT_FAILURE);
  }

  return fd;
}