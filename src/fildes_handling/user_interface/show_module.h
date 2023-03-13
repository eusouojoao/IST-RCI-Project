#ifndef SHOW_MODULES_H
#define SHOW_MODULES_H

#include "../../common/struct.h"
#include "user_commands.h"

void show_wrapper(host *host, int opt);
void show_topology(host *myhost);
void show_names(host *myhost);
void show_routes(host *myhost);

#endif // !SHOW_MODULES_H
