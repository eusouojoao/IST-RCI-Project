#include "utils.h"
#include "../error_handling/error_checking.h"
#include "../protocols/TCP.h"

#include <stdio.h> //APAGAR!!
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NETSIZE 3
#define IDSIZE 2

/**
 * @brief  inicializa host
 * @note   quando se cria o nó (host) bck e o exp é o proprio nó (representado
 * por NULL)
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
 * @brief
 * @note
 * @param  *host:
 * @param  *ID:
 * @param  *net:
 * @retval None
 */
void assign_ID_and_net(host *host, char *ID, char *net) {
  host->ID = (char *)malloc((IDSIZE + 1) * sizeof(char));
  if (host->ID == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  host->net = (char *)malloc((NETSIZE + 1) * sizeof(char));
  if (host->net == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  if (sprintf(host->ID, "%2s", ID) < IDSIZE) {
    /*error*/ exit(EXIT_FAILURE);
  }
  if (sprintf(host->net, "%3s", net) < NETSIZE) {
    /*error*/ exit(EXIT_FAILURE);
  }

  host->tab_expedicao[atoi(ID)] = atoi(ID);
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
 * @brief  cria e insere um nó na lista de nodes do host
 * @note   inserção no inicio da node list
 * @param  ID: ID do node a ser inserido
 * @param  fd: fd assoaciado ao node a ser inserido
 * @param  *IP: endereço do node a ser inserido
 * @param  TCP: porto do node a ser inserido
 * @param  *host: struct host onde está a lista de nodes
 * @retval None
 */
void insert_node(char *ID, int fd, char *IP, int TCP, host *host) {
  node *new_node = create_new_node(ID, fd, IP, TCP);

  new_node->next = host->node_list;
  host->node_list = new_node;

  // caso o host tenha acabado de entrar na rede ou perdido o externo
  if (host->ext == NULL) {
    host->ext = new_node;
  }
  // APAGAR!!
  // podia se logo atualizar a tabela de expedição (mas n sei se é o pretendido)
  // host->tab_expedicao[ID] = ID;
}

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
 * @brief  adiciona uma nova rota à tabela de expedição
 * @note
 * @param  dest: node de dest
 * @param  neighbour: node que tem de se enviar mensagem para chegar ao dest
 * @param  *host: struct host (guarda toda a informação)
 * @retval None
 */

void add_route_tab(int dest, int neighbour, host *host) {
  // APAGAR!! // apenas para testar se as tabelas estão a ser corretamente apagadas
  if (host->tab_expedicao[dest] == -1) {
    printf("ERROR, tabela populada a ser repopulada!");
    exit(1);
  }
  host->tab_expedicao[dest] = neighbour;
}

/**
 * @brief  promove um vertice interno a externo
 * @note   return poderá fazer return NULL caso nao hajam nós internos
 * @param  *host: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
void promote_intr_to_ext(host *host) {
  if (host->ext == NULL) {
    host->ext = host->node_list;
  }
}

/**
 * @brief  promove um vertice de backup a vertice externo
 * @note   return poderá fazer return NULL caso nao haja backup
 *         ao promover o bck a externo temos de o adicionar à lista de nodes
 * @param  *host: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
void promote_bck_to_ext(host *host) {
  if (host->bck != NULL) {
    host->ext = host->bck;
    host->ext->next = host->node_list; // APAGAR TA MAL!!
    host->node_list = host->ext;
  }
  host->bck = NULL;
}

/**
 * @brief  encontrar se um conteudo especido existe
 * @note
 * @param  *name: conteudo que se procura
 * @param  *host: struct host com a lista de names
 * @retval 1 Sucesso, conteudo encontrado
 *         0 Falha, não existia conteudo com tal nome
 *         -1 Falha, name a procurar demasiado longo (logo não poderia tar na
 * lista)
 */
int find_name(char *name, host *host) {
  // APAGAR- não sei se é suposto devolver o conteúdo caso seja
  // encontrado ou apenas uma msg a dizer q o conteudo existe
  // const char *FindName()
  names *list_pointer = host->names_list;
  if (check_name(name) == -1)
    return -1; // Falha, name demasiado longo
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0)
      return 1; // Sucesso, name encontrado
    list_pointer = list_pointer->next;
  }
  return 0; // Falha, não existia name com tal nome
}

/**
 * @brief  Procura na tabela de expedição se existe uma rota para o destino ID
 * @note   Pode retornar NULL caso não exista informação do vizinho
 * @param  *ID: ID do destino
 * @param  *myH: struct com a tabela e lista de nodes
 * @retval retorna apontador para a node vizinha do destino (caso essa informação
 * exista) retorna NULL caso não exista informação da node vizinha
 */
node *check_rote(char *ID, host *myH) {
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
