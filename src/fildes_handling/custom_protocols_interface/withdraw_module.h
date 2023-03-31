/******************************************************************************
 *
 * File: withdraw_module.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling node withdrawals, updating backups, and removing nodes
 *              from the forwarding table.
 *
 *****************************************************************************/
#ifndef WITHDRAW_MODULE_H
#define WITHDRAW_MODULE_H

#include "../../essentials/struct.h"

void withdraw_wrapper(host *host, node *node, char *buffer);
char *remove_node_from_forwarding_table(host *host, int eraseN);

#endif // WITHDRAW_MODULE_H
