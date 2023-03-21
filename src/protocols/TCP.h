#ifndef TCP_utils_H
#define TCP_utils_H

#include <sys/types.h>

#include "../common/struct.h"

ssize_t send_msg_TCP(int fd, char *msg_to_send, size_t msglen);
ssize_t recv_msg_TCP(int fd, char *buffer, size_t size);
char *send_message_TCP(int fd, char *msg);
char *fetch_bck(host *host, char *msg);
int create_listen_socket(user_args *uip);

#endif // !TCP_utils_H
