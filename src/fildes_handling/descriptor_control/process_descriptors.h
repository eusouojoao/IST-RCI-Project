/******************************************************************************
 *
 * File: process_descriptors.h
 *
 * Description: This header file contains the declarations of functions for
 *              processing various types of descriptors, including keyboard
 *              input, new connections, and neighbor nodes.
 *
 *****************************************************************************/
#ifndef PROCESS_DESCRIPTORS_H
#define PROCESS_DESCRIPTORS_H

#include "../../essentials/struct.h"
#include "../custom_protocols_interface/protocol_commands.h"
#include "../user_interface/user_commands.h"

int process_keyboard_input(host *host, char *buffer);
void process_new_connection(host *host, new_connection *connection);
void process_neighbour_nodes(host *host, node *node, char *buffer);

#endif // PROCESS_DESCRIPTORS_H
