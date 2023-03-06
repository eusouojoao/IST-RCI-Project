#include "../hdr/user_interface.h"
#include "../hdr/UDP_utils.h"
#include "../hdr/error_handling.h"
#include "../hdr/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNODES 99

void djoin(char *buffer, host **host, int flag) {
  if (flag == JOIN) {
    /* do stuff */
    return;
  }

  buffer = buffer;
  host = host;

  return;
}

char *fetch_extern_from_nodelist(char *nodelist) {
  char *token = strtok(nodelist, "\n"), *array[MAXNODES] = {NULL};
  int i = 0;

  for (token = strtok(NULL, "\n"); token != NULL; token = strtok(NULL, "\n"), i++) {
    array[i] = token;
  }

  if (i == 0) {
    return NULL; // é o primeiro nó na lista do servidor
  }

  return array[rand() % i];
}

int check_net_and_id(char *net, char *id) {
  if (strlen(net) != 3 || strlen(id) != 2) {
    /*error*/ printf("Enrro! join() args 1\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(net) && check_if_number(id))) {
    /*error*/ printf("Enrro! join() args 2\n");
    return EXIT_FAILURE;
  }

  int int_net = atoi(net), int_id = atoi(id);
  printf("int_net = %d, int_id = %d\n", int_net, int_id);
  if ((int_net < 0 || int_net > 999) || (int_id < 0 || int_id > 99)) {
    /*error*/ printf("Enrro! join() args 3\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; // valores dentro do range
}

void join(char *buffer, host **host) {
  char *received_reg_msg = NULL, *received_nodeslist = NULL, *ext_node = NULL;
  char msg_to_send[128] = {'\0'}, net[32] = {'\0'}, id[32] = {'\0'};
  if (sscanf(buffer, "join %s %s", net, id) < 2) {
    system_error("In join() ->" RED " sscanf() failed");
    return;
  }

  if (check_net_and_id(net, id) == EXIT_FAILURE) {
    return; /* error */
  }

  /* Check if host is already in a network */
  if ((*host)->net != NULL) {
    if (strcmp((*host)->net, net) == 0) {
      /* já se encontra na rede escolhida pelo utilizador */;
    } else {
      /* encontra-se numa rede (*host)->net */;
    }
    return;
  }

  sprintf(msg_to_send, "NODES %s", net);
  received_nodeslist = send_user_message_UDP((*host)->uip, msg_to_send);
  if (received_nodeslist == NULL) {
    return;
  }
  printf("%s", received_nodeslist);

  memset(msg_to_send, 0, strlen(msg_to_send));
  sprintf(msg_to_send, "REG %s %s %s %d", net, id, (*host)->uip->IP,
          (*host)->uip->TCP);
  received_reg_msg = send_user_message_UDP((*host)->uip, msg_to_send);
  if (received_reg_msg == NULL) {
    free(received_nodeslist);
    return;
  }
  printf("%s\n", received_reg_msg);

  /*! TODO: CHECK for OKREG */
  if (strcmp(received_reg_msg, "OKREG") == 0) {
    memset(msg_to_send, 0, strlen(msg_to_send));
    if ((ext_node = fetch_extern_from_nodelist(received_nodeslist)) != NULL) {
      sprintf(msg_to_send, "djoin %s %s %s", net, id, ext_node);
      djoin(msg_to_send, host, JOIN); // connects to the ext node in the network
    }
  } else {
    /* ??? */;
  }

  free(received_nodeslist);
  free(received_reg_msg);
  return;
}

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
  } else if (strcmp(token, "clear") == 0) {
    return CLEAR;
  }

  return UNDEF;
}

void process_stdin_input(char *buffer, host **host) {
  char token[32];
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_stdin_input() ->" RED " sscanf() failed");
    return;
  }

  switch (get_user_command(token)) {
  case JOIN:
    join(buffer, host);
    break;
  case DJOIN:
    djoin(buffer, host, DJOIN);
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
  case CLEAR:
    CLEAR_STDIN();
    printf(BLUE "\tUser interface [" GREEN "ON" BLUE "]\n" RESET);
  case UNDEF:
  default:
    /*error*/ // exit(EXIT_FAILURE);
    break;
  }

  return;
}
