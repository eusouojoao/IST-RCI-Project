#include "delete_node_module.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../core/TCP.h"
#include "common.h"
#include "withdraw_module.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Delete a node from the host's node_list and update the external node if
 * necessary.
 * @param host: pointer to the host structure
 * @param withdraw_fd: file descriptor of the node to be deleted
 */
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

      broadcast_protocol_message(host, withdraw_fd, withdraw_msg);

      if (withdraw_fd != host->ext->fd) {
        free_node(current_node);
      }

      free(withdraw_msg);
      break;
    }
    previous_pointer = current_node;
    current_node = current_node->next;
  }

  update_external_node(host, withdraw_fd);
}

/**
 * @brief Update the external node of the host after a node with the specified file
 * descriptor has been deleted.
 * @param host: pointer to the host structure
 * @param withdraw_fd: file descriptor of the node that was deleted
 */
void update_external_node(host *host, int withdraw_fd) {
  if (host->ext == NULL || host->ext->fd != withdraw_fd) {
    return;
  }

  anchor_status anchor = NO;
  free_node(host->ext);
  host->ext = NULL;

  if (host->bck == NULL) {
    promote_intr_to_ext(host);
    anchor = YES;
  } else {
    promote_bck_to_ext(host);
    if (!get_a_new_backup(host)) {
      delete_node(host, host->ext->fd);
      return;
    }
    anchor = NO;
  }

  notify_internal_nodes_of_external_change(host, anchor);
}

/**
 * @brief Promote an internal node to an external node.
 * @note Used when the external node leaves and the backup node is NULL.
 * @param host: pointer to the host structure
 */
void promote_intr_to_ext(host *host) {
  if (host->ext == NULL) {
    host->ext = host->node_list;
  }
}

/**
 * @brief Promote a backup node to an external node.
 * @note The node must be inserted into the node_list.
 * @param host: pointer to the host structure
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

/**
 * @brief Retrieve a new backup node for the host.
 * @param host: pointer to the host structure
 */
int get_a_new_backup(host *host) {
  char msg_to_send[128] = {'\0'};
  sprintf(msg_to_send, "NEW %s %s %d\n", host->ID, host->uip->IP, host->uip->TCP);
  if (fetch_bck(host, msg_to_send) == -1) {
    return 0;
  }
  return 1;
}

/**
 * @brief Notify internal nodes of changes to the external node.
 * @param host: pointer to the host structure
 */
void notify_internal_nodes_of_external_change(host *host, anchor_status anchor) {
  if (host->ext == NULL) {
    return;
  }

  char msg_to_send[128] = {'\0'};
  sprintf(msg_to_send, "EXTERN %s %s %d\n", host->ext->ID, host->ext->IP, host->ext->TCP);

  // Notify internal nodes
  broadcast_protocol_message(host, host->ext->fd, msg_to_send);

  // The promoted internal node (new external node), must also be notified
  // if the host is not an anchor node
  if (anchor == YES) {
    if (write_msg_TCP(host->ext->fd, msg_to_send, strlen(msg_to_send)) == -1) {
      system_error("write() failed");
    }
  }
}
