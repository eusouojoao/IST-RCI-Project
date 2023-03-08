#include "leave_module.h"
#include "../common/utils.h"
#include "../error_handling/error_messages.h"
#include "../protocols/UDP.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 16

void clear_host(host *host) {
  node *aux1 = host->node_list, *del1 = NULL;
  names *aux2 = host->names_list, *del2 = NULL;

  free(host->net), free(host->ID);
  host->net = host->ID = NULL;

  if (host->ext != NULL && host->ext->fd != -1) {
    close(host->ext->fd);
  }

  free_node(host->bck);
  while (aux1 != NULL) { // delete nodes_list
    del1 = aux1;
    aux1 = aux1->next;
    free_node(del1);
  }
  host->ext = host->bck = NULL;

  while (aux2 != NULL) { // delete names_list
    del2 = aux2;
    aux2 = aux2->next;
    free(del2);
  }

  return;
}

void leave_network(host *host, int flag) {
  /*! TODO: Verificar se o host se encontra numa rede, caso contrário, não há nada a
   * fazer, retornar e avisar o utilizador */
  if (host->net == NULL) {
    printf(YELLOW "[NOTICE]" RESET " Host not registered in a network\n");
    return;
  }

  if (flag == DJOIN) {
    clear_host(host);
    return;
  }

  /* Unregister from the node server */
  char msg_to_send[SIZE] = {'\0'}, *msg_received = NULL;

  sprintf(msg_to_send, "UNREG %s %s\n", host->net, host->ID);
  msg_received = send_message_UDP(host->uip, msg_to_send);

  if (strcmp(msg_received, "OKUNREG") == 0) {
    /*! TODO: Enviar mensagem para os nós vizinhos a avisar: WITHDRAW */
    UDP_server_message(0, msg_received);
    clear_host(host); // limpar a estrutura relativa à rede à qual se despede
  } else {
    /* failed to unregister */;
    UDP_server_message(1, msg_received);
  }

  if (msg_received != NULL) {
    free(msg_received);
  }

  return;
}

void exit_program(host *host, int flag) {
  leave_network(host, flag);

  if (host->ext != NULL) { // Failed to unregister from the network in leave_network()
    clear_host(host);
  }

  close(host->listen_fd);
  // free(host->uip->IP), free(host->uip->regIP);
  free(host->uip);
  free(host);

  printf(BLUE "%*s User interface [" RED "OFF" BLUE "]\n" RESET, 6, "");
  exit(EXIT_SUCCESS);
}
