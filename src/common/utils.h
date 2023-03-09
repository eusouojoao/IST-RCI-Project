#ifndef UTILS_H
#define UTILS_H

#include "struct.h"

host *init_host(user_args *uip);
void assign_ID_and_net(host *host, char *ID, char *net);
node *create_new_node(char *ID, int fd, char *IP, int TCP);
// void delete_host(host *host);
void insert_node(char *ID, int fd, char *IP, int TCP, host *host);
void free_node(node *node);
// void delete_node(char *ID, host *host);
void add_route_tab(int dest, int neighbour, host *host);
void remove_route_tab(int eraseN, host *host);
names *new_names(char *name, names *next);
int insert_name(char *name, host *host);
int delete_name(char *delname, host *host);
int find_name(char *name, host *host);
void promote_intr_to_ext(host *host);
void promote_bck_to_ext(host *host);

#endif
