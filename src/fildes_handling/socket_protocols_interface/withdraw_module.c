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
  host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
  return;
}

char *remove_node_from_forwarding_table(host *host, int eraseN) {
  int deleted_entries = 0;
  char *node_to_withdraw = calloc(SIZE, sizeof(char));
  if (node_to_withdraw == NULL) {
    /*error*/ exit(1);
  }

  snprintf(node_to_withdraw, strlen(node_to_withdraw) + 1, "WITHDRAW %02d\n",
           eraseN);

  for (size_t i = 0; i < ELEMENTS; i++) {
    if (host->tab_expedicao[i] == eraseN) {
      host->tab_expedicao[i] = -1;
      deleted_entries++;
    }
  }

  /*! TODO: Perguntar se faz sentido n~ao enviar mensagem de WITHDRAW no caso de n~ao
   * conhecer o nó (os seguintes também n~ao devem conhecer)*/
  // if (deleted_entries == 0) {
  //   free(node_to_withdraw);
  //   return NULL;
  // }

  return node_to_withdraw;
}

void send_withdraw_messages(host *host, int sender_fd, char *withdraw_msg) {
  node *current_node = host->node_list;
  while (current_node != NULL) {
    if (current_node->fd == sender_fd) {
      continue;
    }
    if (write(current_node->fd, withdraw_msg, strlen(withdraw_msg) + 1) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }
    current_node = current_node->next;
  }
  free(withdraw_msg);
}
