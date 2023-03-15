#ifndef LEAVE_MODULE_H
#define LEAVE_MODULE_H

#include "../../common/struct.h"
#include "user_commands.h"

void clear_host(host *host);
void leave_network(host *host, int flag);
int exit_program(host *host, int flag);

#endif // !LEAVE_MODULE_H
