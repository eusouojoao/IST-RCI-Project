#include "withdraw_module.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "../protocols/TCP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 64

void update_backup(host *host, char *buffer) {
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  sscanf(buffer, "EXTERN %s %s %s", node_ID, node_IP, node_TCP);
  if (check_node_parameters(node_ID, node_IP, node_TCP) == EXIT_FAILURE) {
    /* error */
    printf("Enrro! Bad format!\n");
    return;
  }

  free_node(host->bck);
  host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
  return;
}

void withdraw_node(host *host, char *ID) {
  node *previous_pointer = NULL, *node_list = host->node_list;
  char msg_to_send[128] = {'\0'}, *msg_received = NULL;
  char bckID[64] = {'\0'}, bckIP[64] = {'\0'}, bckTCP[64] = {'\0'}; /*! TODO - buffers*/
  char ID_ext[16] = {'\0'};
  strcpy(ID_ext, host->ext->ID);

  for (; node_list != NULL; previous_pointer = node_list, node_list = node_list->next) {
    if (strcmp(node_list->ID, ID) == 0) {
      if (previous_pointer == NULL) {
        host->node_list = node_list->next;
      } else {
        previous_pointer->next = node_list->next;
      }
      free_node(node_list);
      remove_route_tab(atoi(ID), host); // atualizar tabela de expedição
      break;
    }
  }

  if (strcmp(ID_ext, ID) == 0) { // vai perder nó externo
    host->ext = NULL;
    if (host->bck == NULL) {
      /* Caso nó âncora */
      promote_intr_to_ext(host);
    } else if (host->bck != NULL) {
      promote_bck_to_ext(host);
      /* Fetch new backup from the new extern */
      sprintf(msg_to_send, "NEW %s %s %d\n", ID, host->uip->IP, host->uip->TCP);
      msg_received = fetch_bck(host, msg_to_send);
      if (msg_received == NULL) {
        /*! TODO - error  */
        return;
      }

      sscanf(msg_received, "EXTERN %s %s %s", bckID, bckIP, bckTCP);
      if (check_node_parameters(bckID, bckIP, bckTCP) == EXIT_FAILURE) {
        /* error */
        printf("Error! Bad format!\n");
        return;
      }
      if (strcmp(bckID, host->ID) != 0) {
        host->bck = create_new_node(bckID, -1, bckIP, atoi(bckTCP));
      }
    }
    /* Call all intern nodes to update their backup (current node's extern) */
    memset(&msg_to_send, 0, sizeof(msg_to_send));
    sprintf(msg_to_send, "EXTERN %s %s %d", host->ext->ID, host->ext->IP, host->ext->TCP);
    // the extern node is always the first in the node list, and must be bypassed
    for (node *temp = host->node_list->next; temp != NULL; temp = temp->next) {
      if (write(temp->fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
        system_error("In withdraw_module() ->" RED " write() failed");
      }
    }
  }

  return;
}
