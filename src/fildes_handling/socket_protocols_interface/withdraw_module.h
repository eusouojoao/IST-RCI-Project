
#ifndef WITHDRAW_MODULE_H
#define WITHDRAW_MODULE_H

#include "../../common/struct.h"

void update_backup(host *host, char *buffer);
char *remove_node_from_forwarding_table(host *host, int eraseN);
void send_withdraw_messages(host *host, int sender_fd, char *withdraw_msg);

#endif // !WITHDRAW_MODULE_H
