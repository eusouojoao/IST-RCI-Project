#ifndef COMMON_SOCKET_UTILS_H
#define COMMON_SOCKET_UTILS_H

#include "../../common/struct.h"

void broadcast_protocol_message(host *host, int sender_fd, char *protocol_msg);
int find_name(char *name, host *host);

#endif
