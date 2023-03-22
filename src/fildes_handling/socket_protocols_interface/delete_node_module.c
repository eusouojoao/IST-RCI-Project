#include "delete_node_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../core/TCP.h"
#include "common.h"
#include "withdraw_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*! TODO */
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

      broadcast_protocol_message(host, withdraw_fd, withdraw_msg);
      if (withdraw_fd != host->ext->fd) {
        free_node(current_node);
      }
      break;
    }
    previous_pointer = current_node;
    current_node = current_node->next;
  }

  update_external_node(host, withdraw_fd);
}

/*! TODO */
void update_external_node(host *host, int withdraw_fd) {
  if (host->ext == NULL || host->ext->fd != withdraw_fd) {
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

/**
 * @brief  promote a intern node to extern
 * @note   used when extern node leaves (and backup is NULL)
 * @param  *host: structure host
 * @retval None
 */
void promote_intr_to_ext(host *host) {
  if (host->ext == NULL) {
    host->ext = host->node_list;
  }
}

/**
 * @brief  promote a backup to extern node
 * @note   the node must be insert in the node_list
 * @param  *host: structure host
 * @retval None
 */
void promote_bck_to_ext(host *host) {
  if (host->bck != NULL) {
    host->ext = host->bck;
    host->ext->next = host->node_list;
    host->node_list = host->ext;
    insert_in_forwarding_table(host, atoi(host->ext->ID), atoi(host->ext->ID));
  }

  host->bck = NULL;
}

/*! TODO */
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
    insert_in_forwarding_table(host, atoi(host->bck->ID), atoi(host->ext->ID));
  }
}

/*! TODO */
void notify_internal_nodes_of_external_change(host *host) {
  if (host->ext == NULL) {
    return;
  }

  char msg_to_send[128] = {'\0'};
  sprintf(msg_to_send, "EXTERN %s %s %d", host->ext->ID, host->ext->IP, host->ext->TCP);

  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    if (temp->fd == host->ext->fd) {
      continue;
    }

    if (write(temp->fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      system_error("write() failed");
    }
  }

  /* The promoted internal node (new external node), must also be notified */
  if (host->bck == NULL) {
    if (write(host->ext->fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      system_error("write() failed");
    }
  }
}
