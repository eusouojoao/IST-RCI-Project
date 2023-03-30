/******************************************************************************
 *
 * File: query_module.h
 *
 * Description: This header file contains the declarations of functions for
 *              processing queries and handling content responses in the system.
 *
 *****************************************************************************/
#ifndef QUERY_MODULE_H
#define QUERY_MODULE_H

#include "../../essentials/struct.h"
#include "protocol_commands.h"

void process_query(host *host, node *sender, char *buffer);
void handle_content_response(host *host, node *sender, char *buffer, protocol_command cmd);

#endif // !QUERY_MODULE_H
