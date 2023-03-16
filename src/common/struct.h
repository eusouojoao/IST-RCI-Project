/******************************************************************************
 *
 * File: structs.h
 *
 * Description: This header file contains the data structures used throughout
 *              the program. It includes structures for user input arguments,
 *              nodes in the network, names, and host information.
 *
 *****************************************************************************/

#ifndef STRUCTS_H
#define STRUCTS_H

// Structure to hold user input arguments
typedef struct user_args {
  char *IP;               // IP address of the user
  int TCP;                // TCP port number
  char *regIP;            // IP address of the registration server
  int regUDP;             // UDP port number of the registration server
} user_args;

// Structure to represent a node in the network
typedef struct node {
  char *ID;               // Identifier of the node
  int fd;                 // File descriptor of the connected socket
  char *IP;               // IP address of the node
  int TCP;                // TCP port number of the node

  struct node *next;      // Pointer to the next node in the list
} node;

// Structure to represent a list of names
typedef struct names {
  char *name;             // Name of the content available at the host
  struct names *next;     // Pointer to the next name in the list
} names;

// Structure to represent the host information and state
typedef struct host {
  char *ID;               // Identifier of the host
  char *net;              // Network address of the host
  int tab_expedicao[100]; // Array to store the forwarding table

  user_args *uip;         // Pointer to the user input arguments structure

  int listen_fd;          // File descriptor of the listening socket

  node *bck;              // Pointer to the backup node (not in the node_list)
  node *ext;              // Pointer to the external node (also available in the node_list)
  node *node_list;        // List of neighbor nodes, i.e., internal and external

  names *names_list;      // List of names associated with the host
} host;

#endif // !STRUCTS_H
