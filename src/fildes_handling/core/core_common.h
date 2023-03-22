#ifndef CORE_COMMON_H
#define CORE_COMMON_H

#include <arpa/inet.h>

typedef enum socket_type {
  UDP,
  TCP,
} socket_type;

int create_socket(socket_type type);
int setup_sockaddr_in(struct sockaddr_in *addr, const char *ip, int port);

#endif
