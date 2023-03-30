#ifndef JOIN_MODULES_H
#define JOIN_MODULES_H

#include "../../essentials/struct.h"
#include "user_commands.h"

int join_network(char *buffer, host *host);
int djoin_network(char *buffer, host *host, user_command flag);

#endif // !JOIN_MODULES_H
