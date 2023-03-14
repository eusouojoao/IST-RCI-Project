#ifndef CONTENT_MODULES_H
#define CONTENT_MODULES_H

#include "../../common/struct.h"
#include "user_commands.h"

names *new_names(char *name, names *next);
int insert_name(char *name, host *host);
int delete_name(char *delname, host *host);

#endif // !CONTENT_MODULES_H
