/******************************************************************************
 *
 * File: leave_module.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling the 'leave' and 'exit' user commands, as well as
 *              cleaning up resources in the system.
 *
 *****************************************************************************/
#ifndef LEAVE_MODULE_H
#define LEAVE_MODULE_H

#include "../../essentials/struct.h"
#include "user_commands.h"

void delete_names_list(host *host);
void reset_forwarding_table(host *host);
void delete_nodes_list(host *host);
void clear_host(host *host);
void leave_network(host *host, user_command flag);
int exit_program(host *host, user_command flag);

#endif // LEAVE_MODULE_H
