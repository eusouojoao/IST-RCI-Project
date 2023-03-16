#include "withdraw_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128
#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])

void update_backup(host *host, char *buffer) {
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  sscanf(buffer, "EXTERN %s %s %s", node_ID, node_IP, node_TCP);
  if (check_node_parameters(node_ID, node_IP, node_TCP) == EXIT_FAILURE) {
    /* error */
    return;
  }

  free_node(host->bck);
  if (strcmp(host->ID, node_ID) != 0) {
    host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
    insert_in_forwarding_table(host, atoi(node_ID), atoi(node_ID));
  } else {
    host->bck = NULL;
  }

  return;
}

void withdraw_wrapper(host *host, node *node, char *buffer) {
  char ID[32] = {'\0'};
  // If the command is WITHDRAW, withdraw the node with the specified ID
  if (sscanf(buffer, "WITHDRAW %s", ID) < 1) {
    system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
    return;
  }

  if (!(strlen(ID) == 2 && check_if_number(ID))) {
    /* error, protocol with bad format */
    printf("Error! Bad format!\n");
    return;
  }

  char *withdraw_msg = remove_node_from_forwarding_table(host, atoi(ID));
  if (withdraw_msg == NULL) {
    return;
  }

  send_protocol_messages(host, node->fd, withdraw_msg);
}

char *remove_node_from_forwarding_table(host *host, int eraseN) {
  char *withdraw_message = calloc(SIZE, sizeof(char));
  if (withdraw_message == NULL) {
    /*error*/ exit(1);
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
