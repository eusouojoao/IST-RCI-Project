#include "struct.h"

// user_args *argsInit(char *IP, int TCP, char *regIP, int regUDP);
host *hostInit(int ID, user_args *uip);
node *newnode(int ID, int fd, char *IP, int TCP, node *next);
