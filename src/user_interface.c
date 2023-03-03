#include "../hdr/user_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*! TODO: Implementar com strtok */
void check_stdin_input(char *buffer) {
  char *token = strtok(buffer, " ");
  const char *ap[] = {"join", "djoin", "create", "delete", "get", "show", "leave", "exit", NULL};
  const char **p = ap;

  printf("1st token: %s\n", token);
  while (*p) {
    if (strcmp(token, *p) == 0) {
      printf("Comando inserido: %s\n", *p);
      break;
    }
    p++;

    /* ... */
  }

  /* ... */

  return;
}
