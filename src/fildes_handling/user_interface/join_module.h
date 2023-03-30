/******************************************************************************
 *
 * File: join_modules.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling the 'join' and 'djoin' user commands in the system.
 *
 *****************************************************************************/
#ifndef JOIN_MODULES_H
#define JOIN_MODULES_H

#include "../../essentials/struct.h"
#include "user_commands.h"

int join_network(char *buffer, host *host);
int djoin_network(char *buffer, host *host, user_command flag);

#endif // JOIN_MODULES_H
