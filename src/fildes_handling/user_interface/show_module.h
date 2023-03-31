/******************************************************************************
 *
 * File: show_modules.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling the 'clear' user command in the system.
 *
 *****************************************************************************/
#ifndef SHOW_MODULES_H
#define SHOW_MODULES_H

#include "../../essentials/struct.h"
#include "user_commands.h"

void clear_wrapper(host *host, user_command cmd, char *buffer);

#endif // SHOW_MODULES_H
