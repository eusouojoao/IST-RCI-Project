#include "withdraw_module.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 128
#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])

/**
 * @brief Handle the WITHDRAW command and update the forwarding table.
 * @param host: pointer to the host structure
 * @param sender: pointer to the node that sent the WITHDRAW command
 * @param buffer: buffer containing the WITHDRAW command and the node ID to be
 * withdrawn
 */
void withdraw_wrapper(host *host, node *sender, char *buffer) {
  char ID[32] = {'\0'};
  // If the command is WITHDRAW, withdraw the node with the specified ID
  if (sscanf(buffer, "WITHDRAW %s\n", ID) != 1) {
    system_error("sscanf() failed");
    return;
  }

  if (!check_id(ID)) {
    return;
  }

  char *withdraw_msg = remove_node_from_forwarding_table(host, atoi(ID));
  if (withdraw_msg == NULL) {
    return;
  }

  broadcast_protocol_message(host, sender->fd, withdraw_msg);
  free(withdraw_msg);
}

/**
 * @brief Remove a node from the host's forwarding table.
 * @param host: pointer to the host structure
 * @param eraseN: node ID to be removed from the forwarding table
 * @return a pointer to the WITHDRAW message to be broadcasted to other nodes, NULL on a
 * failure
 */
char *remove_node_from_forwarding_table(host *host, int eraseN) {
  char *withdraw_message = calloc(SIZE, sizeof(char));
  if (withdraw_message == NULL) {
    system_error("calloc() failed");
    return NULL;
  }

  snprintf(withdraw_message, SIZE, "WITHDRAW %02d\n", eraseN);

  host->tab_expedicao[eraseN] = -1;
  for (size_t i = 0; i < ELEMENTS; i++) {
    if (host->tab_expedicao[i] == eraseN) {
      host->tab_expedicao[i] = -1;
    }
  }

  return withdraw_message;
}
