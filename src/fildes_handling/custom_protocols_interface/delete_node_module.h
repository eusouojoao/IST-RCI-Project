/******************************************************************************
 *
 * File: delete_node_module.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling node deletions and updates, such as deleting nodes,
 *              updating external nodes, promoting nodes, and notifying internal
 *              nodes of changes.
 *
 *****************************************************************************/
#ifndef DELETE_NODE_MODULE_H
#define DELETE_NODE_MODULE_H

#include "../../essentials/struct.h"

void delete_node(host *host, int withdraw_fd);
void update_external_node(host *host, int withdraw_fd);
void promote_intr_to_ext(host *host);
void promote_bck_to_ext(host *host);
int get_a_new_backup(host *host);
void notify_internal_nodes_of_external_change(host *host);

#endif // !DELETE_NODE_MODULE_H
