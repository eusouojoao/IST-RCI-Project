#include "join_modules.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "../protocols/TCP.h"
#include "../protocols/UDP.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535
#define MAXNODES 99
#define SIZE 128

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
