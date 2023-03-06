#include "../hdr/utils.h"

#include <stdio.h> //APAGAR!!
#include <stdlib.h>
#include <string.h>

#if 1
/**
 * @brief  inicializa host
 * @note   quando se cria o nó (host) bck e o exp é o proprio nó (representado
 * por NULL)
 * @param  *uip: argumentos do utilizador
 * @retval apontador para o próprio host criado
 */
host *init_host(user_args *uip) {
  host *newH = (host *)malloc(sizeof(struct host));
  if (newH == NULL) {
    /*error*/ exit(0);
  }

  newH->ID = NULL;
  newH->uip = uip;
  newH->bck = NULL; // NULL representa que o proprio nó é o backup
  newH->ext = NULL; // ... e vizinho externo
  newH->node_list = NULL;
  newH->names_list = NULL;
  memset(newH->tab_expedicao, -1, sizeof(newH->tab_expedicao));

  return newH;
}

/**
 * @brief  cria um novo node
 * @note
 * @param  ID: ID do novo node
 * @param  fd: fd associado à ligação com o novo node
 * @param  *IP: endereço do novo node
 * @param  TCP: porto do novo node
 * @param  *next: apontador para o proximo elemento da lista de nodes
 * @retval apontador para a própria node criada
 */
node *new_node(char *ID, int fd, char *IP, int TCP, node *next) {
  node *newN = (node *)malloc(sizeof(struct node));
  if (newN == NULL) {
    /*error*/ exit(0);
  }
  newN->ID = (char *)malloc((strlen(ID) + 1) * sizeof(char));
  if (newN->ID == NULL) {
    /*error*/ exit(0);
  }
  strcpy(newN->ID, ID);

  newN->fd = fd;
  newN->TCP = TCP;
  newN->IP = (char *)malloc((strlen(IP) + 1) * sizeof(char));
  if (newN->IP == NULL) {
    /*error*/ exit(0);
  }
  strcpy(newN->IP, IP);
  newN->next = next; // novo node passa a ser o 1º da lista

  return newN;
}

/**
 * @brief  apaga completamente o host
 * @note
 * @param  *myH: struct host a ser apagada
 * @retval None
 */
void delete_host(host *myH) {
  node *del = NULL;
  node *aux = NULL;
  names *del2 = NULL;
  names *aux2 = NULL;
  aux = myH->node_list;
  aux2 = myH->names_list;
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
  free(myH->uip); // delete user_args
  free(myH);      // delete host
}

/**
 * @brief  cria e insere um nó na lista de nodes do host
 * @note   inserção no inicio da node list
 * @param  ID: ID do node a ser inserido
 * @param  fd: fd assoaciado ao node a ser inserido
 * @param  *IP: endereço do node a ser inserido
 * @param  TCP: porto do node a ser inserido
 * @param  *myH: struct host onde está a lista de nodes
 * @retval None
 */
void insert_node(char *ID, int fd, char *IP, int TCP, host *myH) {
  node *firstN = myH->node_list;
  node *newN = new_node(ID, fd, IP, TCP, firstN);
  myH->node_list = newN;
  if (myH->ext ==
      NULL) // caso o host tenha acabado de entrar na rede ou perdido o externo
    myH->ext = newN;
  // APAGAR!!
  // podia se logo atualizar a tabela de expedição (mas n sei se é o pretendido)
  // myH->tab_expedicao[ID] = ID;
}

/**
 * @brief  adiciona uma nova rota à tabela de expedição
 * @note
 * @param  destino: node de destino
 * @param  vizinho: node que tem de se enviar mensagem para chegar ao destino
 * @param  *myH: struct host (guarda toda a informação)
 * @retval None
 */
void addRote_tab(int destino, int vizinho, host *myH) {
  // APAGAR!!
  if (myH->tab_expedicao[destino] ==
      -1) // apenas para testar se as tabelas estão a ser corretamente apagadas
  {
    printf("ERROR, tabela populada a ser repopulada!");
    exit(1);
  }
  myH->tab_expedicao[destino] = vizinho;
}

/**
 * @brief  remove todas as rotas da tabela de expedição associadas ao node
 * eraseN
 * @note
 * @param  eraseN: node a apagar as rotas
 * @param  *myH: struct host que contém a tabela de expedição
 * @retval None
 */
void removeRote_tab(int eraseN, host *myH) {
  int i;
  myH->tab_expedicao[eraseN] = -1; // remove rote to deleted node
  for (i = 0; i < 100; i++) {
    if (myH->tab_expedicao[i] == eraseN)
      myH->tab_expedicao[i] =
          -1; // remove rote to a node when the destiny is the deleted node
  }
}

/**
 * @brief  apaga uma node específica dada pelo seu ID da lista de nodes
 * @note   atualiza também a tabela de expedição
 * @param  ID: ID da node a ser eliminada
 * @param  *myH: struct host com a lista de nodes
 * @retval None
 */
void delete_node(char *ID, host *myH) {
  node *nodeList = myH->node_list;
  node *previous_pointer = NULL;
  if (myH->ext->ID == ID) // vai perder nó externo
    myH->ext = NULL;
  if (myH->bck->ID == ID) // vai perder nó backup
    myH->bck = NULL;
  while (nodeList != NULL) {
    if (nodeList->ID == ID) {

      if (previous_pointer == NULL)
        myH->node_list = nodeList->next;
      else
        previous_pointer->next = nodeList->next;
      free(nodeList);
      removeRote_tab(atoi(ID), myH); // atualizar tabela de expedição
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
 * @param  *myH: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
node *promoteIntr_to_Ext(host *myH) {
  myH->ext = myH->node_list;
  return myH->ext; // poderá fazer return NULL caso nao hajam nós internos
}

/**
 * @brief  promove um vertice de backup a vertice externo
 * @note   return poderá fazer return NULL caso nao haja backup
 * @param  *myH: struct com a informação dos vertíces
 * @retval apontador para o novo vertice externo
 */
node *promoteBck_to_Ext(host *myH) {
  myH->ext = myH->bck;
  myH->bck = NULL;
  return myH->ext; // poderá fazer return NULL caso nao haja backup
}

/**
 * @brief  cria um novo conteúdo (name)
 * @note
 * @param  *name: conteudo a ser criado
 * @param  *next: apontador pro proximo conteudo
 * @retval apontador para conteudo novo
 */
names *new_names(char *name, names *next) {
  names *newName = (names *)malloc(sizeof(struct names));
  if (newName == NULL) {
    /*error*/ exit(0);
  }
  strcpy(newName->name, name);
  newName->next = next; // novo name passa a ser o 1º da lista

  return newName;
}

/**
 * @brief  insere um novo conteudo na lista de conteudos
 * @note   inserção no inicio da names list
 * @param  *name: conteudo a ser inserido
 * @param  *myH: struct host com a lista de conteudos
 * @retval 1 Sucesso
 *         0 Falha, já existia um name com esse nome
 *         -1 Falha, name demasiado longo
 */
int write_name(char *name, host *myH) {
  names *list_pointer = myH->names_list;
  if (strlen(name) + 1 > 100) // APAGAR - substituir 100 por algo q faça o
                              // sizeof(list_pointer->name)
    return -1;                // Falha, name demasiado longo
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0)
      return 0; // Falha, já existia um name com esse nome
    else
      list_pointer = list_pointer->next;
  }
  names *newName = new_names(name, myH->names_list);
  myH->names_list = newName;
  return 1; // Sucesso
}

/**
 * @brief  apaga um conteudo especifico da lista de conteudos
 * @note
 * @param  *delname: nome a ser apagado
 * @param  *myH: struct host com a lista de names
 * @retval 1 Sucesso
 *         0 Falha, não existia name com tal nome
 *         -1 Falha, name a apagar demasiado longo (logo não poderia tar na
 * lista)
 */
int delete_name(char *delname, host *myH) {
  names *list_pointer = myH->names_list;
  names *previous_pointer = NULL;
  if (strlen(delname) + 1 > 100) // APAGAR - substituir 100 por algo q faça o
                                 // sizeof(list_pointer->name)
    return -1; // Falha, name a apagar demasiado longo (logo não poderia tar na
               // lista)
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, delname) == 0) {
      if (previous_pointer == NULL)
        myH->names_list = list_pointer->next;
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
 * @param  *myH: struct host com a lista de names
 * @retval 1 Sucesso, conteudo encontrado
 *         0 Falha, não existia conteudo com tal nome
 *         -1 Falha, name a procurar demasiado longo (logo não poderia tar na
 * lista)
 */
int find_name(
    char *name,
    host *myH) { // APAGAR- não sei se é suposto devolver o conteúdo caso seja
                 // encontrado ou apenas uma msg a dizer q o conteudo existe
  // const char *FindName()
  names *list_pointer = myH->names_list;
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
