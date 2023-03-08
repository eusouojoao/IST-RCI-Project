#include "leave_module.h"
#include "../common/utils.h"
#include "../error_handling/error_messages.h"
#include "../protocols/UDP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

void clear_host(host *host) {
  node *aux1 = host->node_list, *del1 = NULL;
  names *aux2 = host->names_list, *del2 = NULL;

  free(host->net), free(host->ID);
  host->net = host->ID = NULL;

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

void leave_network(host *host) {
  char msg_to_send[SIZE] = {'\0'}, *msg_received = NULL;

  /*! TODO: Verificar se o host já se encontra numa rede, caso contrário, não há nada a
   * fazer, retornar e avisar o utilizador */

  /* Unregister from the node server */
  sprintf(msg_to_send, "UNREG %s %s\n", host->net, host->ID);
  msg_received = send_message_UDP(host->uip, msg_to_send);

  if (strcmp(msg_received, "OKUNREG") == 0) {
    /*! TODO: Enviar mensagem para os nós vizinhos a avisar: WITHDRAW */

    fprintf(stdout, "[Successfully unregistered from the network %s]\n", host->net);
    clear_host(host); // limpar a estrutura relativa à rede à qual se despede
  } else {
    /* failed to unregister */;
    fprintf(stderr, "[Something went wrong :(");
    fprintf(stderr, "Couldn't unregister from the network %s]\n", host->net);
  }

  if (msg_received != NULL) {
    free(msg_received);
  }

  return;
}

void exit_network(host *host) {
  leave_network(host);

  if (host->ext != NULL) { // Failed to unregister from the network in leave_network()
    clear_host(host);
  }

  close(host->listen_fd);
  // free(host->uip->IP), free(host->uip->regIP);
  free(host->uip);
  free(host);

  printf(BLUE "\tUser interface [" RED "OFF" BLUE "]\n" RESET);
  exit(EXIT_SUCCESS);
}
