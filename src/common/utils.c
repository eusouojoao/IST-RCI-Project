#include "utils.h"
#include "../error_handling/error_checking.h"
#include "../fildes_handling/core/TCP.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief  inicializa host
 * @note   quando se cria o nó (host) bck e o exp é o proprio nó (representado
 *         por NULL)
 * @param  *uip: argumentos do utilizador
 * @retval apontador para o próprio host criado
 */
host *init_host(user_args *uip) {
  host *new_host = (host *)malloc(sizeof(host));
  if (new_host == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  new_host->ID = NULL;
  new_host->net = NULL;

  new_host->uip = uip;
  new_host->bck = NULL; // NULL representa que o proprio nó é o backup
  new_host->ext = NULL; // ... e neighbour externo

  new_host->node_list = NULL;
  new_host->names_list = NULL;
  memset(new_host->tab_expedicao, -1, sizeof(new_host->tab_expedicao));

  return new_host;
}

/**
 * @brief  cria um novo node
 * @note
 * @param  ID: ID do novo node
 * @param  fd: fd associado à ligação com o novo node
 * @param  *IP: endereço do novo node
 * @param  TCP: porto do novo node
 * @retval apontador para a própria node criada
 */
node *create_new_node(char *ID, int fd, char *IP, int TCP) {
  node *new_node = (node *)malloc(sizeof(struct node));
  if (new_node == NULL) {
    /*error*/ exit(0);
  }
  new_node->ID = (char *)malloc((strlen(ID) + 1) * sizeof(char));
  if (new_node->ID == NULL) {
    /*error*/ exit(0);
  }
  strcpy(new_node->ID, ID);

  new_node->fd = fd;
  new_node->TCP = TCP;
  new_node->IP = (char *)malloc((strlen(IP) + 1) * sizeof(char));
  if (new_node->IP == NULL) {
    /*error*/ exit(0);
  }
  strcpy(new_node->IP, IP);
  new_node->next = NULL;

  return new_node;
}

/**
 * @brief  Insert (and create) a new node in the node_list
 * @note   Insertion in list is made in the head
 * @param  *ID: ID of the node to be inserted
 * @param  fd: fd of the node to be inserted
 * @param  *IP: IP of the node to be inserted
 * @param  TCP: TCP of the node to be inserted
 * @param  *host: Host structure where the node_list information resides
 * @retval None
 */
void insert_node(char *ID, int fd, char *IP, int TCP, host *host) {
  node *new_node = create_new_node(ID, fd, IP, TCP);

  new_node->next = host->node_list;
  host->node_list = new_node;

  if (host->ext == NULL) {
    host->ext = new_node;
  }
}

/**
 * @brief  Free a struct of the type node
 * @note
 * @param  *node: structure to be free
 * @retval None
 */
void free_node(node *node) {
  if (node != NULL) {
    free(node->ID);
    free(node->IP);
    if (node->fd != -1)
      close(node->fd);
    free(node);
  }
  return;
}

/**
 * @brief Update the forwarding table with the destination and corresponding
 * neighbor.
 *
 * This function updates the host's forwarding table, associating the destination
 * with its corresponding neighbor.
 *
 * @param dest: The destination node's ID.
 * @param neighbour: The neighbor node's ID through which the destination is
 * reachable.
 * @param host: The host structure containing the forwarding table to be updated.
 */
void insert_in_forwarding_table(host *host, int dest, int neighbour) {
  host->tab_expedicao[dest] = neighbour;
}

/**
 * @brief  Verify if the host knows the rote to another node (with specific ID)
 * @note
 * @param  *myH: structure host
 * @param  *ID: ID of the node we are searching for the rote (destiny)
 * @retval NULL - there is no information in the expedition table
 *         auxN - neighbor node to where we must send the information (rote to destiny)
 */
node *check_route(host *myH, char *ID) {
  int destino = atoi(ID), vizinho = -1;
  node *auxN = NULL;

  if (myH->tab_expedicao[destino] != -1) {
    vizinho = myH->tab_expedicao[destino];
    auxN = myH->node_list;

    while (auxN != NULL) {
      if (atoi(auxN->ID) == vizinho)
        return auxN; // retorna a node vizinha
      auxN = auxN->next;
    }
  }

  return auxN; // se retornar auxN==NULL, ent não encontrou o node vizinha na tabela
}
