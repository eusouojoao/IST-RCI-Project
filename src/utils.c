#include "../hdr/utils.h"
#include <stdlib.h>
#include <string.h>

host *hostInit(int ID, user_args *uip) {
  host *newH = (host *)malloc(sizeof(struct host));
  if (newH == NULL) {
    /*error*/ exit(0);
  }

  newH->ID = ID;
  newH->uip = uip;
  newH->bck = NULL; // NULL representa que o proprio nó é o backup
  newH->ext = NULL; // ... e vizinho externo
  newH->node_list = NULL;
  newH->names_list = NULL;
  memset(newH->tab_expedicao, -1, sizeof(newH->tab_expedicao));

  return newH;
}

node *newnode(int ID, int fd, char *IP, int TCP, node *next) {
  node *newN = (node *)malloc(sizeof(struct node));
  if (newN == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  newN->ID = ID;
  newN->fd = fd;
  newN->TCP = TCP;
  newN->IP = (char *)malloc((strlen(IP) + 1) * sizeof(char));
  if (newN->IP == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }

  // strcpy(IP, newN->IP);
  newN->next = next; // novo node passa a ser o 1º da lista

  return newN;
}
