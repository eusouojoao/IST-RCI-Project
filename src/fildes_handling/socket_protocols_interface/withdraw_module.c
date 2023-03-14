#include "withdraw_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"

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
  } else {
    host->bck = NULL;
  }
  return;
}

char *remove_node_from_forwarding_table(host *host, int eraseN) {
  char *node_to_withdraw = calloc(SIZE, sizeof(char));
  if (node_to_withdraw == NULL) {
    /*error*/ exit(1);
  }

  snprintf(node_to_withdraw, strlen(node_to_withdraw) + 1, "WITHDRAW %02d\n",
           eraseN);

  for (size_t i = 0; i < ELEMENTS; i++) {
    if (host->tab_expedicao[i] == eraseN) {
      host->tab_expedicao[i] = -1;
    }
  }

  return node_to_withdraw;
}

void send_withdraw_messages(host *host, int sender_fd, char *withdraw_msg) {
  node *current_node = host->node_list;
  size_t len = strlen(withdraw_msg) + 1;
  while (current_node != NULL) {
    if (current_node->fd == sender_fd) {
      continue;
    }

    if (write(current_node->fd, withdraw_msg, len) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }

    current_node = current_node->next;
  }

  free(withdraw_msg);
}
