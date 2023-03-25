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

#ifndef CBUFFER_SIZE
#define CBUFFER_SIZE 256
#endif // !CBUFFER_SIZE

#include <time.h>

// Circular buffer structure
typedef struct circular_buffer {
  char data[CBUFFER_SIZE]; // The buffer data
  size_t read_pos;         // The current read position
  size_t write_pos;        // The current write position
  size_t current_size;     // The current size of the buffer
} circular_buffer_t;

typedef struct new_connection {
  int new_fd;
  circular_buffer_t *cb;
  time_t last_activity; // Time of the last activity

  struct new_connection *next;
} new_connection;

// Structure to hold user input arguments
typedef struct user_args {
  char *IP;    // IP address of the user
  int TCP;     // TCP port number
  char *regIP; // IP address of the registration server
  int regUDP;  // UDP port number of the registration server
} user_args;

// Structure to represent a node in the network
typedef struct node {
  char *ID; // Identifier of the node
  char *IP; // IP address of the node
  int TCP;  // TCP port number of the node

  int fd;                // File descriptor of the connected socket
  circular_buffer_t *cb; // Pointer to the circular buffer associated with the socket

  struct node *next; // Pointer to the next node in the list
} node;

// Structure to represent a list of names
typedef struct names {
  char *name;         // Name of the content available at the host
  struct names *next; // Pointer to the next name in the list
} names;

// Structure to represent the host information and state
typedef struct host {
  char *net;              // Network address of the host
  char *ID;               // Identifier of the host
  int tab_expedicao[100]; // Array to store the forwarding table

  user_args *uip; // Pointer to the user input arguments structure

  int listen_fd; // File descriptor of the listening socket
  new_connection *new_connections_list;

  node *bck;       // Pointer to the backup node (not in the node_list)
  node *ext;       // Pointer to the external node (also available in the node_list)
  node *node_list; // List of neighbor nodes, i.e., internal and external

  names *names_list; // List of names associated with the host
} host;

#endif // !STRUCTS_H
