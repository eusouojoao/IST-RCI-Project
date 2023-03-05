#include "../hdr/user_interface.h"
#include "../hdr/UDP_utils.h"
#include "../hdr/error_handling.h"
#include "../hdr/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void djoin(char *buffer, host **host) { return; }

char *fetch_extern_from_nodelist(char *nodelist) {
  char *token = strtok(nodelist, "\n");
  int i = 0;

  for (; nodelist[i]; nodelist[i] == '\n' ? i++ : *nodelist++) {
    /* ... */
  }
  for (int j = rand() % i; --j && token != NULL; token = strtok(NULL, "\n")) {
    /* ... */
  }

  return NULL;
}

void join(char *buffer, host **host) {
  char *received_reg_msg = NULL, *received_nodeslist = NULL, *ext_node = NULL;
  char msg_to_send[64] = {'\0'}, net[32] = {'\0'}, id[32] = {'\0'};
  if (sscanf(buffer, "join %s %s", net, id) < 2) {
    system_error("In join() ->" RED " sscanf() failed");
    return;
  }

  if (!(check_if_number(net) && check_if_number(id))) {
    /*error*/ printf("Enrro! join() args\n");
    return;
  }

  /* Check if host is already in a network */
  if ((*host)->net != -1) {
    if ((*host)->net == atoi(net)) {
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

  memset(msg_to_send, 0, strlen(msg_to_send));
  sprintf(msg_to_send, "REG %s %s %s %d", net, id, (*host)->uip->IP,
          (*host)->uip->TCP);
  received_reg_msg = send_user_message_UDP((*host)->uip, msg_to_send);
  if (received_reg_msg == NULL) {
    free(received_nodeslist);
    return;
  }

  /*! TODO: CHECK for OKREG */
  if (strcmp(received_reg_msg, "OKREG") == 0) {
    memset(msg_to_send, 0, strlen(msg_to_send));
    if ((ext_node = fetch_extern_from_nodelist(received_nodeslist)) == NULL) {
      sprintf(msg_to_send, "djoin %s %s %s", net, id, "");
      djoin(msg_to_send, host); // connects to the ext node in the network
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
  }

  return UNDEF;
}

void process_stdin_input(char *buffer, host **host) {
  char token[32];
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_stdin_input() ->" RED " sscanf() failed");
    return;
  }

  printf("token: %s\n", token);

  switch (get_user_command(token)) {
  case JOIN:
    join(buffer, host);
    break;
  case DJOIN:
    djoin(buffer, host);
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
