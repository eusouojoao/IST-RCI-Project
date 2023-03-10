#include "show_module.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief  Imprime a topologia associada ao host
 * @note   ID e net do host; ID dos Vizinhos externos internos e backup
 * @param  *myhost: struct com a informação do host
 * @retval None
 */
void show_topology(host *myhost) {
  char *auxext = myhost->ext->ID;
  node *pointer = myhost->node_list; /*!TODO - editar os printf*/
  printf("Topologia do host com: ID %s; Rede %s \n", myhost->ID, myhost->net);
  printf("Nó de backup %s; Vizinho Externo %s\n", myhost->bck->ID, myhost->ext->ID);
  printf("Lista de Vizinhos Internos:");
  while (pointer != NULL) {
    if (strcmp(pointer->ID, auxext) != 0) // não imprimir nó externo como um interno
      printf(" %s;", pointer->ID);
    pointer = pointer->next;
  }
  printf("\n------------------------\n");
}

/**
 * @brief  Imprime os conteudos (nomes) do host
 * @note
 * @param  *myhost: struct com a informação do host
 * @retval None
 */
void show_names(host *myhost) {
  names *pointer = myhost->names_list; /*!TODO - editar os printf*/
  printf("Nomes do host com: ID %s; Rede %s \n", myhost->ID, myhost->net);
  printf("Lista de Nomes:");
  while (pointer != NULL) {
    printf(" %s;", pointer->name);
    pointer = pointer->next;
  }
  printf("\n------------------------\n");
}

void show_routes(host *myhost) {
  int i; /*!TODO - editar os printf*/
  printf("Rotas do host com: ID %s; Rede %s \n", myhost->ID, myhost->net);
  printf("Tabela de Expedição: \n");
  for (i = 0; i < 100; i++) {
    if (myhost->tab_expedicao[i] != -1)
      printf("Destino: %d --> Vizinho: %d \n", i, myhost->tab_expedicao[i]);
  }
  printf("------------------------\n");
}
