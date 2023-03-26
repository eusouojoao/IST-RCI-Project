#ifndef NEW_CONNECTIONS_LIST_H
#define NEW_CONNECTIONS_LIST_H

#include "../common/struct.h"

void insert_new_connection(host *host, int new_fd, char *buffer);
int remove_new_connection(host *host, int new_fd);
void clean_inactive_connections(host *host);

#endif // !NEW_CONNECTIONS_LIST_H
