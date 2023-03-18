#ifndef JOIN_MODULES_H
#define JOIN_MODULES_H

#include "../../common/struct.h"

void check_uniqueness_of_ID(host *host, char *node_list, char (*ID)[64]);
void assign_host_ID_and_network(host *host, const char *ID, const char *net);
int join_network(char *buffer, host *host);
char *fetch_extern_from_nodelist(char *nodelist);
int djoin_network(char *buffer, host *host, int flag);

#endif // !JOIN_MODULES_H
