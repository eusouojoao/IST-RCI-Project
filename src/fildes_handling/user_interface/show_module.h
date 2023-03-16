#ifndef SHOW_MODULES_H
#define SHOW_MODULES_H

#include "../../common/struct.h"
#include "user_commands.h"

void show_wrapper(host *host, int opt, char *buffer);
void show_topology(host *host);
void show_names(host *host);
void show_routes(host *host);

#endif // !SHOW_MODULES_H
