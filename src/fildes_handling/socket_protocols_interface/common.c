#include "common.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void broadcast_protocol_message(host *host, int sender_fd, char *protocol_msg) {
  // Initialize a pointer to the first node in the network
  node *current_node = host->node_list;
  // Calculate the length of the protocol message string + the null terminator
  size_t len = strlen(protocol_msg) + 1;
  // Iterate over all neighbour nodes
  while (current_node != NULL) {
    if (current_node->fd == sender_fd) { // Skip the sender node
      current_node = current_node->next;
      continue;
    }

    if (write(current_node->fd, protocol_msg, len) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }

    current_node = current_node->next;
  }

  free(protocol_msg);
}

void send_message_to_neighbours(host *host, char *dest, char *protocol_msg) {
  node *neighbour = check_route(host, dest);
  if (neighbour != NULL) {
    // If the route is known, forward the message to the right neighbour
    if (write(neighbour->fd, protocol_msg, 256) == -1) {
      printf("Error sending QUERY to known neighbor\n");
    }
  } else {
    // Else, it must be broadcasted to all known neighbours
    broadcast_protocol_message(host, -1, protocol_msg);
  }
}

/**
 * @brief  Check if a name exists on a specific host
 * @note
 * @param  *name: name we are looking for
 * @param  *host: structure host
 * @retval 1 - Name found
 *         0 - Name not found
 */
int find_name(char *name, host *host) {
  names *list_pointer = host->names_list;
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0) {
      return 1; // Sucesso, name encontrado
    }

    list_pointer = list_pointer->next;
  }

  return 0; // Falha, não existia name com tal nome
}
