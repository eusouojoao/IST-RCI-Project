#include "delete_node_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../protocols/TCP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])
#define SIZE 128

void delete_node(host *host, int withdraw_fd) {
  node *previous_pointer = NULL, *current_node = host->node_list;
  char *withdraw_msg = NULL;

  while (current_node != NULL) {
    if (current_node->fd == withdraw_fd) {
      if (previous_pointer == NULL) {
        host->node_list = current_node->next;
      } else {
        previous_pointer->next = current_node->next;
      }

      withdraw_msg = remove_node_from_forwarding_table(host, atoi(current_node->ID));
      if (withdraw_msg == NULL) {
        return;
      }

      send_withdraw_messages(host, withdraw_msg);
      free_node(current_node);
      break;
    }
    previous_pointer = current_node;
    current_node = current_node->next;
  }

  update_external_node(host, withdraw_fd);
}

char *remove_node_from_forwarding_table(host *host, int eraseN) {
  char node_to_withdraw[SIZE] = {'\0'};
  char *msg_to_send = calloc(SIZE << 3, sizeof(char));
  if (msg_to_send == NULL) {
    /*error*/ exit(1);
  }

  size_t i = 0;
  while (i < ELEMENTS) {
    if (host->tab_expedicao[i] == eraseN) {
      host->tab_expedicao[i] = -1;
      memset(node_to_withdraw, 0, sizeof(node_to_withdraw));
      snprintf(node_to_withdraw, sizeof(node_to_withdraw), "WITHDRAW %02lu\n", i);
      strcat(msg_to_send, node_to_withdraw);
    }
    i++;
  }

  if (i == 0) {
    return NULL; /* Error: nothing to be done */
  }

  return msg_to_send;
}

void send_withdraw_messages(host *host, char *withdraw_msg) {
  node *current_node = host->node_list;
  while (current_node != NULL) {
    if (current_node->fd == host->ext->fd) {
      continue;
    }
    if (write(current_node->fd, withdraw_msg, strlen(withdraw_msg) + 1) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }
    current_node = current_node->next;
  }
}

void update_external_node(host *host, int withdraw_fd) {
  if (host->ext == NULL || host->ext->fd != withdraw_fd) {
    return;
  }

  host->ext = NULL;

  if (host->bck == NULL) {
    promote_intr_to_ext(host);
  } else {
    promote_bck_to_ext(host);
    get_a_new_backup(host);
  }

  notify_internal_nodes_of_external_change(host);
}

void get_a_new_backup(host *host) {
  char msg_to_send[128] = {'\0'};
  char bckID[64] = {'\0'}, bckIP[64] = {'\0'}, bckTCP[64] = {'\0'};

  sprintf(msg_to_send, "NEW %s %s %d\n", host->ID, host->uip->IP, host->uip->TCP);
  char *msg_received = fetch_bck(host, msg_to_send);
  if (msg_received == NULL) {
    return;
  }

  sscanf(msg_received, "EXTERN %s %s %s", bckID, bckIP, bckTCP);

  if (check_node_parameters(bckID, bckIP, bckTCP) == EXIT_FAILURE) {
    printf("Error! Bad format!\n");
    return;
  }

  if (strcmp(bckID, host->ID) != 0) {
    host->bck = create_new_node(bckID, -1, bckIP, atoi(bckTCP));
  }
}

void notify_internal_nodes_of_external_change(host *host) {
  if (host->ext == NULL) {
    return;
  }

  char msg_to_send[128] = {'\0'};
  sprintf(msg_to_send, "EXTERN %s %s %d", host->ext->ID, host->ext->IP,
          host->ext->TCP);

  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    if (temp->fd == host->ext->fd) {
      continue;
    }

    if (write(temp->fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }
  }
}
