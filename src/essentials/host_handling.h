/******************************************************************************
 *
 * File: host_handling.h
 *
 * Description: This header file contains the declarations of functions for
 *              managing host and node structures. It includes functions for
 *              initializing the host, creating new nodes, inserting nodes,
 *              freeing nodes, and finding nodes by name.
 *
 *****************************************************************************/
#ifndef HOST_HANDLING
#define HOST_HANDLING

#include "struct.h"

host *init_host(user_args *uip);
node *create_new_node(char *ID, int fd, char *IP, int TCP);
void insert_node(char *ID, int fd, char *IP, int TCP, host *host);
void free_node(node *node);
int find_name(char *name, host *host);

void insert_in_forwarding_table(host *host, int dest, int neighbour);
node *check_route(host *host, char *ID);

#endif // HOST_HANDLING
