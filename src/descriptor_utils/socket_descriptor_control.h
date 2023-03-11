#ifndef DESCRIPTOR_CONTROL_H
#define DESCRIPTOR_CONTROL_H

#include "../common/struct.h"
#include "protocol_commands.h"

#include <sys/select.h>

void update_working_set(host *host, fd_set *working_set);
int get_maxfd(host *host);
void process_new_fd(host *host, int new_fd, char *buffer);
protocol_command get_protocol_command(char *token);
void process_neighbour_node_fd(host *host, node *node, char *buffer);

#endif // !DESCRIPTOR_CONTROL_H
