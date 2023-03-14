#include "content_module.h"
#include "../../error_handling/error_checking.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

  if (check_name(name) == -1) {
    return -1; // Falha, name demasiado longo
  }

  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0) {
      return 0; // Falha, já existia um name com esse nome
    } else {
      list_pointer = list_pointer->next;
    }
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
  if (check_name(delname) == -1)
    return -1; // Falha, name demasiado longo
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
