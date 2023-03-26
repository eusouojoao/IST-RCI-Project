#include "withdraw_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "common.h"
#include "delete_node_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128
#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])

/**
 * @brief Update the backup node information in the host structure.
 * @param host: Pointer to the host structure.
 * @param buffer: A buffer containing the new backup node information.
 */
void update_backup(host *host, char *buffer) {
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  free_node(host->bck), host->bck = NULL;

  if (sscanf(buffer, "EXTERN %s %s %s\n", node_ID, node_IP, node_TCP) != 3) {
    /*! TODO: Perguntar o que fazer nesta situaç~ao */
    delete_node(host, host->ext->fd);
    return;
  }

  if (check_node_parameters(node_ID, node_IP, node_TCP) == EXIT_FAILURE) {
    delete_node(host, host->ext->fd);
    return;
  }

  if (strcmp(host->ID, node_ID) != 0) {
    host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
    insert_in_forwarding_table(host, atoi(node_ID), atoi(node_ID));
  }

  return;
}

/**
 * @brief Handle the WITHDRAW command and update the forwarding table.
 * @param host: Pointer to the host structure.
 * @param sender: Pointer to the node that sent the WITHDRAW command.
 * @param buffer: A buffer containing the WITHDRAW command and the node ID to be
 * withdrawn.
 */
void withdraw_wrapper(host *host, node *sender, char *buffer) {
  char ID[32] = {'\0'};
  // If the command is WITHDRAW, withdraw the node with the specified ID
  if (sscanf(buffer, "WITHDRAW %s\n", ID) != 1) {
    system_error("sscanf() failed");
    return;
  }

  if (!(strlen(ID) == 2 && check_if_number(ID))) {
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
 * @param host: Pointer to the host structure.
 * @param eraseN: Node ID to be removed from the forwarding table.
 * @return A pointer to the WITHDRAW message to be broadcasted to other nodes.
 */
char *remove_node_from_forwarding_table(host *host, int eraseN) {
  char *withdraw_message = calloc(SIZE, sizeof(char));
  if (withdraw_message == NULL) {
    exit(1);
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
