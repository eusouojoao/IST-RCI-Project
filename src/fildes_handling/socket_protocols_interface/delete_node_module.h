#ifndef DELETE_NODE_MODULE_H
#define DELETE_NODE_MODULE_H

#include "../../common/struct.h"

void delete_node(host *host, int withdraw_fd);
char *remove_node_from_forwarding_table(host *host, int eraseN);
void send_withdraw_messages(host *host, char *withdraw_msg);
void update_external_node(host *host, int withdraw_fd);
void get_a_new_backup(host *host);
void notify_internal_nodes_of_external_change(host *host);

#endif
