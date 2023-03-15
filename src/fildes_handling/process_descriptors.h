#ifndef PROCESS_DESCRIPTORS_H
#define PROCESS_DESCRIPTORS_H

#include "../common/struct.h"
#include "socket_protocols_interface/protocol_commands.h"
#include "user_interface/user_commands.h"

user_command get_user_command(char *token);
int process_keyboard_input(host *host, char *buffer);

void process_new_connection(host *host, int new_fd, char *buffer);

protocol_command get_protocol_command(char *token);
void process_neighbour_nodes(host *host, node *node, char *buffer);

#endif // !PROCESS_DESCRIPTORS_H
