#ifndef JOIN_MODULES_H
#define JOIN_MODULES_H

#include "../../common/struct.h"

int join_network(char *buffer, host *host);
char *fetch_extern_from_nodelist(char *nodelist);
int djoin_network(char *buffer, host *host, int flag);

#endif // !JOIN_MODULES_H
