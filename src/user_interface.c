#include "../hdr/user_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void djoin() { return; }

void join() { return; }

user_command get_user_command(char *token) {
  if (strcmp(token, "join") == 0) {
    return JOIN;
  } else if (strcmp(token, "djoin") == 0) {
    return DJOIN;
  } else if (strcmp(token, "create") == 0) {
    return CREATE;
  } else if (strcmp(token, "delete") == 0) {
    return DELETE;
  } else if (strcmp(token, "get") == 0) {
    return GET;
  } else if (strcmp(token, "show") == 0) {
    return SHOW;
  } else if (strcmp(token, "leave") == 0) {
    return LEAVE;
  } else if (strcmp(token, "exit") == 0) {
    return EXIT;
  }

  return UNDEF;
}

void process_stdin_input(char *buffer) {
  char *token = strtok(buffer, " ");

  switch (get_user_command(token)) {
  case JOIN:
    join();
    break;
  case DJOIN:
    djoin();
    break;
  case CREATE:
    /*! TODO: */
    break;
  case DELETE:
    /*! TODO: */
    break;
  case GET:
    /*! TODO: */
    break;
  case SHOW:
    /*! TODO: */
    break;
  case LEAVE:
    /*! TODO: */
    break;
  case EXIT:
    /*! TODO: */
    break;
  case UNDEF:
  default:
    /*error*/ // exit(EXIT_FAILURE);
    break;
  }

  return;
}
