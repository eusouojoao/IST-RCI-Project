#ifndef TCP_utils_H
#define TCP_utils_H

#include <stdio.h>
#include <sys/types.h>

#include "../../essentials/struct.h"

ssize_t write_msg_TCP(int fd, char *msg_to_send, size_t msglen);
ssize_t fetch_bck(host *host, char *msg);
int create_listen_socket(user_args *uip);

#endif // !TCP_utils_H
