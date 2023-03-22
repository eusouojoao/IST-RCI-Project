#ifndef LEAVE_MODULE_H
#define LEAVE_MODULE_H

#include "../../common/struct.h"
#include "user_commands.h"

void delete_names_list(host *host);
void reset_forwarding_table(host *host);
void delete_nodes_list(host *host);
void clear_host(host *host);
void leave_network(host *host, user_command flag);
int exit_program(host *host, user_command flag);

#endif // !LEAVE_MODULE_H
