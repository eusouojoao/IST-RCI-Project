#ifndef UTILS_H
#define UTILS_H

#include "struct.h"

host *init_host(user_args *uip);
node *new_node(char *ID, int fd, char *IP, int TCP, node *next);
void delete_host(host *myH);
void insert_node(char *ID, int fd, char *IP, int TCP, host *myH);
void delete_node(char *ID, host *myH);
void addRote_tab(int destino, int vizinho, host *myH);
void removeRote_tab(int eraseN, host *myH);
names *new_names(char *name, names *next);
int write_name(char *name, host *myH);
int delete_name(char *delname, host *myH);
int find_name(char *name, host *myH);
node *promote_Intr_to_Ext(host *myH);
node *promote_Bck_to_Ext(host *myH);

#endif
