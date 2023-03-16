#include "query_module.h"
#include "../../error_handling/error_checking.h"

#include <string.h>

int find_name(char *name, host *host) {
  // APAGAR- não sei se é suposto devolver o conteúdo caso seja
  // encontrado ou apenas uma msg a dizer q o conteudo existe
  // const char *FindName()
  if (check_name(name) == -1) {
    return -1; // Falha, name demasiado longo
  }

  names *list_pointer = host->names_list;
  while (list_pointer != NULL) {
    if (strcmp(list_pointer->name, name) == 0) {
      return 1; // Sucesso, name encontrado
    }

    list_pointer = list_pointer->next;
  }

  return 0; // Falha, não existia name com tal nome
}
