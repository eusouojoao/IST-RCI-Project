#ifndef TCP_utils_H
#define TCP_utils_H

#include "../common/struct.h"

char *send_message_TCP(int fd, char *msg);
char *fetch_bck(host *host, char *msg);
int create_listen_socket(user_args *uip);

#endif // !TCP_utils_H
