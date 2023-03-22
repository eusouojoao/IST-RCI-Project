#ifndef CLEAR_MODULES_H
#define CLEAR_MODULES_H

#include "../../common/struct.h"
#include "user_commands.h"

void show_wrapper(host *host, user_command cmd, char *buffer);
void show_topology(host *host);
void show_names(host *host);
void show_routes(host *host);

#endif // !CLEAR_MODULES_H
