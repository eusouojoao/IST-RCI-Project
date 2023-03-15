
#ifndef WITHDRAW_MODULE_H
#define WITHDRAW_MODULE_H

#include "../../common/struct.h"

void update_backup(host *host, char *buffer);
void withdraw_wrapper(host *host, node *node, char *buffer);
char *remove_node_from_forwarding_table(host *host, int eraseN);

#endif // !WITHDRAW_MODULE_H
