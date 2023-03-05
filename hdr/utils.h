#ifndef UTILS_H
#define UTILS_H

#include "struct.h"

host *hostInit(int ID, user_args *uip);
node *newNode(int ID, int fd, char *IP, int TCP, node *next);
void deleteHost(host *myH);
void insertNode(int ID, int fd, char *IP, int TCP, host *myH);
void addRote_tab(int destino, int vizinho, host *myH);
void removeRote_tab(int eraseN, host *myH);
void deleteNode(int ID, host *myH);
names *newNames(char *name, names *next);
int writeName(char *name, host *myH);
int deleteName(char *delname, host *myH);
int findName(char *name, host *myH);
node *promoteVItoVE(host *myH);
node *promoteBcktoVE(host *myH);

#endif
