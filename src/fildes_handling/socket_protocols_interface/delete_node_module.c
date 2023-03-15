#include "delete_node_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../protocols/TCP.h"
#include "common.h"
#include "withdraw_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        printf("WITHDRAW error - teste\n"); // DEBUG - remover
        return;
      }

      send_protocol_messages(host, withdraw_fd, withdraw_msg);
      if (withdraw_fd != host->ext->fd) {
        printf("not here\n");
        free_node(current_node);
      }
      break;
    }
    previous_pointer = current_node;
    current_node = current_node->next;
  }

  update_external_node(host, withdraw_fd);
}

void update_external_node(host *host, int withdraw_fd) {
  if (host->ext == NULL || host->ext->fd != withdraw_fd) {
    printf("update_external_node() error - teste\n"); // DEBUG - remover
    return;
  }

  free_node(host->ext);
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
    printf("get_a_new_backup() error - teste\n"); // DEBUG - remover
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

  /* The promoted internal node (new external node), must also be notified */
  if (host->bck == NULL) {
    if (write(host->ext->fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      system_error("In withdraw_module() ->" RED " write() failed");
    }
  }
}
