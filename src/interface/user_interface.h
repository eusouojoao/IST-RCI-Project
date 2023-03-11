#ifndef user_interface_H
#define user_interface_H

#include "../common/struct.h"
#include "user_commands.h"

user_command get_user_command(char *token);
void process_stdin_input(host *host, char *buffer);

#endif // !user_interface_H
