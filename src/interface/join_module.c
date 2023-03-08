#include "join_module.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "../protocols/TCP.h"
#include "../protocols/UDP.h"
#include "leave_module.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535
#define MAXNODES 99
#define SIZE 64

int djoin_network(char *buffer, host *host, int flag) {
  if (host->ext != NULL) {
    return 0;
  }

  if (number_of_command_arguments(buffer, ' ') > 5) {
    /*! TODO: Treat error: invalid user input */
    return 0;
  }

  char msg_to_send[SIZE << 2] = {'\0'}, *received_msg = NULL;
  char net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};
  char bootID[SIZE] = {'\0'}, bootIP[SIZE] = {'\0'}, bootTCP[SIZE] = {'\0'};

  if (sscanf(buffer, "djoin %s %s %s %s %s", net, ID, bootID, bootIP, bootTCP) < 5) {
    /*! TODO: Treat error: invalid user input or function failure */
    return 0;
  }

  /* If the input comes from join(), all arguments passed to the buffer are
   * properly checked and ready to be used. A direct call from the user to djoin() must
   * have all it's arguments verified */
  if (flag == DJOIN) {
    /*! TODO: Verify input buffer */
    if (check_net_and_id(net, ID) == EXIT_FAILURE ||
        check_boot_parameters(bootID, bootIP, bootTCP) == EXIT_FAILURE) {
      /*error*/ printf("Invalid djoin call\n");
      return 0;
    }
  }

  /* Single node in the network */
  if (strcmp(ID, bootID) == 0) {
    assign_ID_and_net(host, ID, net);
    return 1;
  }

  insert_node(bootID, -1, bootIP, atoi(bootTCP), host);
  assign_ID_and_net(host, ID, net);

  /* Message exchange between the host and the extern node */
  memset(msg_to_send, 0, sizeof(msg_to_send));
  sprintf(msg_to_send, "NEW %s %s %d\n", ID, host->ext->IP, host->ext->TCP);
  printf("msg_to_send: %s", msg_to_send);
  received_msg = fetch_bck(host, msg_to_send);
  if (received_msg == NULL) {
    printf("123\n");
    leave_network(host, flag == JOIN ? JOIN : DJOIN);
    /* error */ return 0;
  }

  sscanf(received_msg, "EXTERN %s %s %s", bootID, bootIP, bootTCP);
  host->bck = create_new_node(bootID, -1, bootIP, atoi(bootTCP));

  return 1;
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
  // printf("array[rand() %% i=%d]: %s\n", i, str); // REMOVER mais tarde
  return str;
}

int join_network(char *buffer, host *host) {
  if (number_of_command_arguments(buffer, ' ') > 2) {
    /*! TODO: Treat error: invalid user input */
    return 0;
  }

  char *received_reg_msg = NULL, *received_nodeslist = NULL, *ext_node = NULL;
  char msg_to_send[SIZE << 2] = {'\0'}, net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};

  if (sscanf(buffer, "join %s %s", net, ID) < 2) {
    system_error("In join() -> sscanf() failed");
    return 0;
  }

  if (check_net_and_id(net, ID) == EXIT_FAILURE) {
    return 0; /* error */
  }

  /* Check if host is already in a network */
  if (host->net != NULL) {
    if (strcmp(host->net, net) == 0) {
      /* já se encontra na rede escolhida pelo utilizador */;
    } else {
      /* encontra-se numa rede host->net */;
    }
    return 0;
  }

  sprintf(msg_to_send, "NODES %s", net);
  received_nodeslist = send_message_UDP(host->uip, msg_to_send);
  if (received_nodeslist == NULL) {
    return 0;
  }

  memset(msg_to_send, 0, sizeof(msg_to_send));
  sprintf(msg_to_send, "REG %s %s %s %d", net, ID, host->uip->IP, host->uip->TCP);
  received_reg_msg = send_message_UDP(host->uip, msg_to_send);
  if (received_reg_msg == NULL) {
    free(received_nodeslist);
    return 0;
  }

  /*! TODO: CHECK for OKREG */
  if (strcmp(received_reg_msg, "OKREG") == 0) {
    UDP_server_message(0, received_nodeslist);
    UDP_server_message(0, received_reg_msg);
    memset(msg_to_send, 0, sizeof(msg_to_send));
    if ((ext_node = fetch_extern_from_nodelist(received_nodeslist)) != NULL) {
      sprintf(msg_to_send, "djoin %s %s %s", net, ID, ext_node);
      djoin_network(msg_to_send, host, JOIN); // connects to the ext node in the network
    } else {
      assign_ID_and_net(host, ID, net);
    }
  } else {
    UDP_server_message(1, received_reg_msg);
  }

  free(received_nodeslist);
  free(received_reg_msg);
  return 1;
}
