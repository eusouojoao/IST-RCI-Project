#include "user_interface.h"
#include "../error_handling/error_messages.h"
#include "content_module.h"
#include "join_module.h"
#include "leave_module.h"
#include "show_module.h"

#include <stdio.h>
#include <string.h>

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
  } else if (strcmp(token, "show") == 0 || strcmp(token, "st") == 0 ||
             strcmp(token, "sn") == 0 || strcmp(token, "sr") == 0) {
    return SHOW;
  } else if (strcmp(token, "leave") == 0) {
    return LEAVE;
  } else if (strcmp(token, "exit") == 0) {
    return EXIT;
  } else if (strcmp(token, "clear") == 0) {
    return CLEAR;
  }

  return UNDEF;
}

void process_stdin_input(char *buffer, host *host) {
  char token[32];
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_stdin_input() ->" RED " sscanf() failed");
    return;
  }

  switch (get_user_command(token)) {
  case JOIN:
    join_network(buffer, host);
    break;
  case DJOIN:
    djoin_network(buffer, host, DJOIN);
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
    leave_network(host);
    break;
  case EXIT:
    exit_network(host);
    break;
  case CLEAR:
    CLEAR_STDIN();
    printf(BLUE "\tUser interface [" GREEN "ON" BLUE "]\n" RESET);
    break;
  case UNDEF:
  default:
    /*error*/ // exit(EXIT_FAILURE);
    break;
  }

  return;
}
