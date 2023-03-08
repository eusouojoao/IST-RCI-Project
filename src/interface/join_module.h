#ifndef JOIN_MODULES_H
#define JOIN_MODULES_H

#include "../common/struct.h"

void join_network(char *buffer, host *host);
char *fetch_extern_from_nodelist(char *nodelist);
void djoin_network(char *buffer, host *host, int flag);

#endif // !JOIN_MODULES_H
