#ifndef UTILS_H
#define UTILS_H

#include "struct.h"

host *init_host(user_args *uip);
node *create_new_node(char *ID, int fd, char *IP, int TCP);
void insert_node(char *ID, int fd, char *IP, int TCP, host *host);
void free_node(node *node);
int find_name(char *name, host *host);

void insert_in_forwarding_table(host *host, int dest, int neighbour);
node *check_rote(host *host, char *ID);

#endif
