#include "utils.h"

#include <stdio.h> //APAGAR!!
#include <stdlib.h>
#include <string.h>

#define NETSIZE 3
#define IDSIZE 2

#if 1
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

void assign_ID_and_net(host *host, char *ID, char *net) {
  host->ID = (char *)malloc((IDSIZE + 1) * sizeof(char));
  if (host->ID == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  host->net = (char *)malloc((IDSIZE + 1) * sizeof(char));
  if (host->ID == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  if (sprintf(host->ID, "%2s", ID) < IDSIZE) {
    /*error*/ exit(EXIT_FAILURE);
  }
  if (sprintf(host->net, "%3s", net) < NETSIZE) {
    /*error*/ exit(EXIT_FAILURE);
  }

  return;
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
 * @brief  apaga completamente o host
 * @note
 * @param  *host: struct host a ser apagada
 * @retval None
 */
void delete_host(host *host) {
  node *del = NULL;
  node *aux = NULL;
  names *del2 = NULL;
  names *aux2 = NULL;
  aux = host->node_list;
  aux2 = host->names_list;
  while (aux != NULL) // delete node_list
  {
    del = aux;
    aux = aux->next;
    free(del);
  }
  while (aux2 != NULL) // delete names_list
  {
    del2 = aux2;
    aux2 = aux2->next;
    free(del2);
  }
  free(host->uip); // delete user_args
  free(host);      // delete host
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
  node *first_node = host->node_list;
  node *new_node = create_new_node(ID, fd, IP, TCP);

  new_node->next = first_node;
  host->node_list = new_node;

  if (host->ext ==
      NULL) // caso o host tenha acabado de entrar na rede ou perdido o externo
    host->ext = new_node;
  // APAGAR!!
  // podia se logo atualizar a tabela de expedição (mas n sei se é o pretendido)
  // host->tab_expedicao[ID] = ID;
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
  // APAGAR!!
  if (host->tab_expedicao[dest] ==
      -1) // apenas para testar se as tabelas estão a ser corretamente apagadas
  {
    printf("ERROR, tabela populada a ser repopulada!");
    exit(1);
  }
  host->tab_expedicao[dest] = neighbour;
}

/**
 * @brief  remove todas as rotas da tabela de expedição associadas ao node
 * eraseN
 * @note
 * @param  eraseN: node a apagar as rotas
 * @param  *host: struct host que contém a tabela de expedição
 * @retval None
 */
void remove_route_tab(int eraseN, host *host) {
  int i;
  host->tab_expedicao[eraseN] = -1; // remove rote to deleted node
  for (i = 0; i < 100; i++) {
    if (host->tab_expedicao[i] == eraseN)
      host->tab_expedicao[i] =
          -1; // remove rote to a node when the destiny is the deleted node
  }
}

/**
 * @brief  apaga uma node específica dada pelo seu ID da lista de nodes
 * @note   atualiza também a tabela de expedição
 * @param  ID: ID da node a ser eliminada
 * @param  *host: struct host com a lista de nodes
 * @retval None
 */
void delete_node(char *ID, host *host) {
  node *nodeList = host->node_list;
  node *previous_pointer = NULL;

  if (host->ext->ID == ID) // vai perder nó externo
    host->ext = NULL;
  if (host->bck->ID == ID) // vai perder nó backup
    host->bck = NULL;

  while (nodeList != NULL) {
    if (nodeList->ID == ID) {

      if (previous_pointer == NULL)
        host->node_list = nodeList->next;
      else
        previous_pointer->next = nodeList->next;

      free(nodeList);
      remove_route_tab(atoi(ID), host); // atualizar tabela de expedição
      return;
    }
    previous_pointer = nodeList;
    nodeList = nodeList->next;
  }
  // APAGAR - apenas para teste
  printf("ERROR, não se encontrou o node a apagar!");
  return; // Falha não encontrou o node a apagar
}

/**
 * @brief  promove um vertice interno a externo
 * @note   return poderá fazer return NULL caso nao hajam nós internos
 * @param  *host: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
void promote_intr_to_ext(host *host) { host->ext = host->node_list; }

/**
 * @brief  promove um vertice de backup a vertice externo
 * @note   return poderá fazer return NULL caso nao haja backup
 * @param  *host: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
void promote_bck_to_ext(host *host) {
  host->ext = host->bck;
  host->bck = NULL;
}

/**
 * @brief  cria um novo conteúdo (name)
 * @note
 * @param  *name: conteudo a ser criado
 * @param  *next: apontador pro proximo conteudo
 * @retval apontador para conteudo novo
 */
names *new_names(char *name, names *next) {
  names *new_nodeame = (names *)malloc(sizeof(struct names));
  if (new_nodeame == NULL) {
    /*error*/ exit(0);
  }
  strcpy(new_nodeame->name, name);
  new_nodeame->next = next; // novo name passa a ser o 1º da lista

  return new_nodeame;
}

/**
 * @brief  insere um novo conteudo na lista de conteudos
 * @note   inserção no inicio da names list
 * @param  *name: conteudo a ser inserido
 * @param  *host: struct host com a lista de conteudos
 * @retval 1 Sucesso
 *         0 Falha, já existia um name com esse nome
 *         -1 Falha, name demasiado longo
 */
int insert_name(char *name, host *host) {
  names *list_pointer = host->names_list;
  if (strlen(name) + 1 > 100) // APAGAR - substituir 100 por algo q faça o
                              // sizeof(list_pointer->name)
    return -1;                // Falha, name demasiado longo
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0)
      return 0; // Falha, já existia um name com esse nome
    else
      list_pointer = list_pointer->next;
  }
  names *new_nodeame = new_names(name, host->names_list);
  host->names_list = new_nodeame;
  return 1; // Sucesso
}

/**
 * @brief  apaga um conteudo especifico da lista de conteudos
 * @note
 * @param  *delname: nome a ser apagado
 * @param  *host: struct host com a lista de names
 * @retval 1 Sucesso
 *         0 Falha, não existia name com tal nome
 *         -1 Falha, name a apagar demasiado longo (logo não poderia tar na
 * lista)
 */
int delete_name(char *delname, host *host) {
  names *list_pointer = host->names_list;
  names *previous_pointer = NULL;
  if (strlen(delname) + 1 > 100) // APAGAR - substituir 100 por algo q faça o
                                 // sizeof(list_pointer->name)
    return -1; // Falha, name a apagar demasiado longo (logo não poderia tar na
               // lista)
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, delname) == 0) {
      if (previous_pointer == NULL)
        host->names_list = list_pointer->next;
      else
        previous_pointer->next = list_pointer->next;
      free(list_pointer);
      return 1; // Sucesso, delname apagado
    } else {
      previous_pointer = list_pointer;
      list_pointer = list_pointer->next;
    }
  }
  return 0; // Falha, não existia name com tal nome
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
int find_name(char *name,
              host *host) { // APAGAR- não sei se é suposto devolver o conteúdo caso seja
                            // encontrado ou apenas uma msg a dizer q o conteudo existe
  // const char *FindName()
  names *list_pointer = host->names_list;
  if (strlen(name) + 1 > 100) // APAGAR - substituir 100 por algo q faça o
                              // sizeof(list_pointer->name)
    return -1; // Falha, name a procurar demasiado longo (logo não poderia tar
               // na lista)
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0)
      return 1; // Sucesso, name encontrado
    list_pointer = list_pointer->next;
  }
  return 0; // Falha, não existia name com tal nome
}

#endif