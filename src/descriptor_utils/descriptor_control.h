#ifndef DESCRIPTOR_CONTROL_H
#define DESCRIPTOR_CONTROL_H

#include "../common/struct.h"

int get_maxfd(host *host);
void process_newfd(host *host, int new_fd, char *buffer);

#endif // !DESCRIPTOR_CONTROL_H
