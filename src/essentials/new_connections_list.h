/******************************************************************************
 *
 * File: new_connections_list.h
 *
 * Description: This header file contains the declarations of functions for
 *              managing a list of new connections. It includes functions for
 *              inserting, removing, and cleaning up inactive new connections.
 *
 *****************************************************************************/
#ifndef NEW_CONNECTIONS_LIST_H
#define NEW_CONNECTIONS_LIST_H

#include "../essentials/struct.h"

void insert_new_connection(host *host, int new_fd, char *buffer);
int remove_new_connection(host *host, int new_fd);

#endif // NEW_CONNECTIONS_LIST_H
