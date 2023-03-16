#include "common.h"
#include "../../error_handling/error_checking.h"
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
