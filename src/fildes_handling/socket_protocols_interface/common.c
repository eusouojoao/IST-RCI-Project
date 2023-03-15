#include "common.h"
#include "../../error_handling/error_messages.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void send_protocol_messages(host *host, int sender_fd, char *protocol_msg) {
  // Initialize a pointer to the first node in the network
  node *current_node = host->node_list;
  // Calculate the length of the protocol message string + the null terminator
  size_t len = strlen(protocol_msg) + 1;
  // Iterate over all neighbour nodes
  while (current_node != NULL) {
    if (current_node->fd == sender_fd) { // Skip the sender node
      continue;
    }

    if (write(current_node->fd, protocol_msg, len) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }

    current_node = current_node->next;
  }

  free(protocol_msg);
}
