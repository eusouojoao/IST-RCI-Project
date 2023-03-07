#include "user_interface.h"
#include "../common/parser.h"
#include "../common/utils.h"
#include "../error_handling/error_messages.h"
#include "../protocols/TCP.h"
#include "../protocols/UDP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535
#define MAXNODES 99
#define SIZE 128

int number_of_command_arguments(char *str, char delim) {
  int n = 0;
  for (int i = 0; str[i]; i++) {
    n += (str[i] == delim);
  }
  return n;
}

int check_boot_parameters(char *bootID, char *bootIP, char *bootTCP) {
  if (strlen(bootID) != 2) {
    /*error*/ printf("strlen\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(bootID) && check_if_number(bootTCP)) ||
      (check_IP_address(bootIP) != 1)) {
    /*error*/ printf("not a number\n");
    return EXIT_FAILURE;
  }

  int int_id = atoi(bootID), int_tcp = atoi(bootTCP);
  printf("int_net = %d, int_id = %d\n", int_id, int_tcp);
  if ((int_id < 0 || int_id > 99) || (int_tcp < 0 || int_tcp > MAXPORT)) {
    /*error*/ printf("out of range\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int check_net_and_id(char *net, char *id) {
  if (strlen(net) != 3 || strlen(id) != 2) {
    /*error*/ printf("strlen\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(net) && check_if_number(id))) {
    /*error*/ printf("not a number\n");
    return EXIT_FAILURE;
  }

  int int_net = atoi(net), int_id = atoi(id);
  printf("int_net = %d, int_id = %d\n", int_net, int_id);
  if ((int_net < 0 || int_net > 999) || (int_id < 0 || int_id > 99)) {
    /*error*/ printf("out of range\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; // valores dentro do range
}

void djoin(char *buffer, host *host, int flag) {
  if (host->ext != NULL) {
    return;
  }

  if (number_of_command_arguments(buffer, ' ') > 5) {
    /*! TODO: Treat error: invalid user input */
    return;
  }

  char msg_to_send[SIZE] = {'\0'}, *received_ext_msg = NULL;
  char net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};
  char bootID[SIZE] = {'\0'}, bootIP[SIZE] = {'\0'}, bootTCP[SIZE] = {'\0'};

  if (sscanf(buffer, "djoin %s %s %s %s %s", net, ID, bootID, bootIP, bootTCP) < 5) {
    /*! TODO: Treat error: invalid user input or function failure */
    return;
  }

  /* If the input comes from join(), all arguments passed to the buffer are
   * properly checked and ready to be used. A direct call from the user to djoin() must
   * have all it's arguments verified */
  if (flag == DJOIN) {
    /*! TODO: Verify input buffer */
    if (check_net_and_id(net, ID) == EXIT_FAILURE ||
        check_boot_parameters(bootID, bootIP, bootTCP) == EXIT_FAILURE) {
      /*error*/ printf("Invalid djoin call\n");
      return;
    }
  }

  /* Single node in the network */
  if (strcmp(ID, bootID) == 0) {
    assign_ID_and_net(host, ID, net);
    return;
  }

  insert_node(bootID, -1, bootIP, atoi(bootTCP), host);

  /* Message exchange between the host and the extern node */
  sprintf(msg_to_send, "NEW %s %s %d\n", ID, host->ext->IP, host->ext->TCP);
  received_ext_msg = fetch_bck(host, msg_to_send);
  if (received_ext_msg == NULL) {
    free(host->ext);
    host->node_list = host->ext = NULL;
    /* error */ return;
  }

  assign_ID_and_net(host, ID, net);

  sscanf(received_ext_msg, "EXTERN %s %s %s", bootID, bootIP, bootTCP);
  host->bck = create_new_node(bootID, -1, bootIP, atoi(bootTCP));

  return;
}

char *fetch_extern_from_nodelist(char *nodelist) {
  char *token = strtok(nodelist, "\n"), *array[MAXNODES] = {NULL};
  int i = 0;
  char *str = NULL;

  for (token = strtok(NULL, "\n"); token != NULL; token = strtok(NULL, "\n"), i++) {
    array[i] = token;
  }

  if (i == 0) {
    return NULL; // é o primeiro nó na lista do servidor
  }

  str = array[rand() % i];
  printf("array[rand() %% i=%d]: %s\n", i, str);
  return str;
}

void join(char *buffer, host *host) {
  if (number_of_command_arguments(buffer, ' ') > 2) {
    /*! TODO: Treat error: invalid user input */
    return;
  }

  char *received_reg_msg = NULL, *received_nodeslist = NULL, *ext_node = NULL;
  char msg_to_send[SIZE] = {'\0'}, net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};

  if (sscanf(buffer, "join %s %s", net, ID) < 2) {
    system_error("In join() ->" RED " sscanf() failed");
    return;
  }

  if (check_net_and_id(net, ID) == EXIT_FAILURE) {
    return; /* error */
  }

  /* Check if host is already in a network */
  if (host->net != NULL) {
    if (strcmp(host->net, net) == 0) {
      /* já se encontra na rede escolhida pelo utilizador */;
    } else {
      /* encontra-se numa rede host->net */;
    }
    return;
  }

  sprintf(msg_to_send, "NODES %s", net);
  received_nodeslist = send_message_UDP(host->uip, msg_to_send);
  if (received_nodeslist == NULL) {
    return;
  }
  printf("%s", received_nodeslist);

  memset(msg_to_send, 0, strlen(msg_to_send));
  sprintf(msg_to_send, "REG %s %s %s %d", net, ID, host->uip->IP, host->uip->TCP);
  received_reg_msg = send_message_UDP(host->uip, msg_to_send);
  if (received_reg_msg == NULL) {
    free(received_nodeslist);
    return;
  }
  printf("%s\n", received_reg_msg);

  /*! TODO: CHECK for OKREG */
  if (strcmp(received_reg_msg, "OKREG") == 0) {
    memset(msg_to_send, 0, strlen(msg_to_send));
    if ((ext_node = fetch_extern_from_nodelist(received_nodeslist)) != NULL) {
      sprintf(msg_to_send, "djoin %s %s %s", net, ID, ext_node);
      djoin(msg_to_send, host, JOIN); // connects to the ext node in the network
    } else {
      assign_ID_and_net(host, ID, net);
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

void process_stdin_input(char *buffer, host *host) {
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
    break;
  case UNDEF:
  default:
    /*error*/ // exit(EXIT_FAILURE);
    break;
  }

  return;
}
