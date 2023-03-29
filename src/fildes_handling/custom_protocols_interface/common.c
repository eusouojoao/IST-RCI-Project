#include "common.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../core/TCP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Broadcasts a protocol message to all neighbors except the sender.
 *
 * This function iterates through the list of neighbor nodes and sends the protocol message to
 * each neighbor, excluding the sender. The message is sent via a TCP connection.
 *
 * @param host: pointer to the host structure.
 * @param sender_fd: the file descriptor of the sender node.
 * @param protocol_msg: the protocol message to be sent to the neighbors.
 */
void broadcast_protocol_message(host *host, int sender_fd, char *protocol_msg) {
  // Initialize a pointer to the first node in the network
  node *current_node = host->node_list;
  // Calculate the length of the protocol message string
  size_t len = strlen(protocol_msg);
  // Iterate over all neighbour nodes
  while (current_node != NULL) {
    if (current_node->fd == sender_fd) { // Skip the sender node
      current_node = current_node->next;
      continue;
    }

    if (write_msg_TCP(current_node->fd, protocol_msg, len) == -1) {
      system_error("write() failed");
    }

    current_node = current_node->next;
  }
}

/**
 * @brief Sends a message to neighbors based on the route information.
 *
 * This function checks if the route to the destination is known. If it is, the message is
 * forwarded to the appropriate neighbor. Otherwise, the message is broadcast to all known
 * neighbours.
 *
 * @param host: pointer to the host structure
 * @param fd: the file descriptor of the sender node
 * @param dest: the destination node ID
 * @param protocol_msg: the protocol message to be sent to the neighbors
 */
void send_message_to_neighbours(host *host, int fd, char *dest, char *protocol_msg) {
  node *neighbour = check_route(host, dest);
  if (neighbour != NULL) {
    // If the route is known, forward the message to the right neighbour
    if (write_msg_TCP(neighbour->fd, protocol_msg, strlen(protocol_msg)) == -1) {
      return;
    }
  } else {
    // Else, it must be broadcasted to all known neighbours
    broadcast_protocol_message(host, fd, protocol_msg);
  }
}

/**
 * @brief Checks if a given name exists on a specific host.
 *
 * This function iterates through the host's names list, searching for the provided name. If
 * the name is found, it returns 1, otherwise 0.
 *
 * @param name: the name to be searched for.
 * @param host: pointer to the host structure.
 * @return 1 if the name is found, 0 if not found.
 */
int find_name(char *name, host *host) {
  names *list_pointer = host->names_list;
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0) {
      return 1; // Success, name was added to the list
    }

    list_pointer = list_pointer->next;
  }

  return 0; // Failure
}
