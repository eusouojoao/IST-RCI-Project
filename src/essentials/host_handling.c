#include "host_handling.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "../essentials/circular_buffer.h"
#include "../fildes_handling/core/TCP.h"
#include "struct.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Initializes a new host structure.
 *
 * This function creates a new host structure and initializes its fields.
 *
 * @param uip: User arguments for the host.
 * @return pointer to the new host structure.
 */
host *init_host(user_args *uip) {
  // Allocate memory for new_host
  host *new_host = (host *)malloc(sizeof(host));
  if (new_host == NULL) {
    system_error("malloc() failed");
    free(uip);
    exit(1);
  }

  // Initialize fields of new_host
  new_host->net = NULL;
  new_host->ID = NULL;
  memset(new_host->tab_expedicao, -1, sizeof(new_host->tab_expedicao));

  new_host->uip = uip;
  new_host->bck = NULL;
  new_host->ext = NULL;

  new_host->new_connections_list = NULL;
  new_host->node_list = NULL;
  new_host->names_list = NULL;

  return new_host;
}

/**
 * @brief Creates a new node structure.
 *
 * This function creates a new node structure and initializes its fields.
 *
 * @param ID: ID of the new node
 * @param fd: file descriptor of the new node
 * @param IP: IP address of the new node
 * @param TCP: TCP port of the new node
 * @return pointer to the new node structure, NULL on failure.
 */
node *create_new_node(char *ID, int fd, char *IP, int TCP) {
  // Allocate memory for new_node
  node *new_node = (node *)malloc(sizeof(node));
  if (new_node == NULL) {
    system_error("malloc() failed");
    return NULL;
  }

  // Initialize fields of new_node
  if ((new_node->ID = strdup(ID)) == NULL) {
    system_error("strdup() failed");
    free(new_node);
    return NULL;
  }
  if ((new_node->IP = strdup(IP)) == NULL) {
    system_error("strdup() failed");
    free(new_node->ID);
    free(new_node);
    return NULL;
  }

  new_node->TCP = TCP;
  new_node->fd = fd;

  new_node->cb = (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
  if (new_node->cb == NULL) {
    system_error("malloc() failed");
    free(new_node->ID), free(new_node->IP);
    free(new_node);
    return NULL;
  }
  cb_init(new_node->cb);

  new_node->next = NULL;
  return new_node;
}

/**
 * @brief Inserts a new node into the host's node list.
 *
 * This function creates a new node structure using the given arguments and
 * inserts it into the host's node list.
 *
 * @param ID: ID of the new node
 * @param fd: file descriptor of the new node
 * @param IP: IP address of the new node
 * @param TCP: TCP port of the new node
 * @param host: host structure where the node list information resides
 */
void insert_node(char *ID, int fd, char *IP, int TCP, host *host) {
  node *new_node = create_new_node(ID, fd, IP, TCP);
  if (new_node == NULL) {
    die_with_system_error(host, NULL);
  }

  new_node->next = host->node_list;
  host->node_list = new_node;

  if (host->ext == NULL) {
    host->ext = new_node;
  }
}

/**
 * @brief Frees a node structure.
 *
 * This function frees the memory associated with a node structure, including the circular
 * buffer.
 *
 * @param node: pointer to the node structure to free.
 */
void free_node(node *node) {
  if (node == NULL) {
    // Nothing to free
    return;
  }

  // Free the fields of the node
  free(node->ID), free(node->IP);
  if (node->fd != -1) {
    close(node->fd);
  }
  free(node->cb);

  // Free the node structure itself
  free(node);
}

/**
 * @brief Inserts an entry in the host's forwarding table.
 *
 * This function updates the host's forwarding table by associating the given destination
 * with its corresponding neighbor.
 *
 * @param host: host structure containing the forwarding table to be updated
 * @param dest: ID of the destination node
 * @param neighbour: ID of the neighbor node through which the destination is reachable
 */
void insert_in_forwarding_table(host *host, int dest, int neighbour) {
  host->tab_expedicao[dest] = neighbour;
}

/**
 * @brief Checks if the host has a route to a node with a specific ID.
 *
 * This function searches the host's forwarding table for an entry corresponding to the given
 * destination node ID. If an entry is found, it returns the node corresponding to the
 * associated neighbor ID. Otherwise, it returns NULL.
 *
 * @param host: host structure containing the forwarding table to search
 * @param ID: ID of the destination node
 * @return pointer to the node corresponding to the associated neighbor ID, or NULL if not
 * found.
 */
node *check_route(host *host, char *ID) {
  int destination = atoi(ID), neighbour = -1;
  node *n = NULL;

  // Check if there is an entry for the destination in the forwarding table
  if (host->tab_expedicao[destination] != -1) {
    neighbour = host->tab_expedicao[destination];
    n = host->node_list;

    // Search the node list for the neighbor node
    while (n != NULL) {
      if (atoi(n->ID) == neighbour) {
        // Return the node corresponding to the neighbor ID
        return n;
      }
      n = n->next;
    }
  }

  // Return NULL if no entry was found
  return n;
}
