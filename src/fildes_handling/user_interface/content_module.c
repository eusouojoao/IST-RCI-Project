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
 * @brief Insert a name in the names list of the host.
 * @param name The name to be inserted.
 * @param host The host where the name should be inserted.
 * @return Returns 1 on success, -1 if the name is too long, 0 if the name already
 * exists in the list.
 */
int insert_name(char *name, host *host) {
  if (check_name(name) == -1) {
    return -1; // Failure, name is too long
  }

  // Check if name already exists in the list
  for (names *current = host->names_list; current != NULL; current = current->next) {
    if (strcmp(current->name, name) == 0) {
      return 0; // Failure, name already exists in the list
    }
  }

  // Insert new name at the beginning of the list
  host->names_list = new_names(name, host->names_list);
  return 1; // Success, name was inserted
}

/**
 * @brief Delete a name from the names list of the host.
 * @param delname The name to be deleted.
 * @param host The host where the name should be deleted from.
 * @return Returns 1 on success, -1 if the name is too long, 0 if the name does not
 * exist in the list.
 */
int delete_name(char *delname, host *host) {
  if (check_name(delname) == -1) {
    return -1; // Name too long
  }

  names **p = &host->names_list;
  while (*p != NULL) {
    if (strcmp((*p)->name, delname) == 0) {
      names *temp = *p;
      *p = (*p)->next;
      free(temp);
      return 1; // Success, name deleted
    }
    p = &(*p)->next;
  }

  return 0; // Failure, name not found
}
