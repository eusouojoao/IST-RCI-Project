#include "join_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../protocols/TCP.h"
#include "../../protocols/UDP.h"
#include "leave_module.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535
#define MAXNODES 99
#define SIZE 64

/**
 * @brief Fetches a random external node from a node list string.
 * @note Assumes the first token in the list the header: NODESLIST XXX.
 *
 * @param nodelist: a string containing the node list, separated by newline
 * characters.
 *
 * @return a pointer to a string containing the needed information of the randomly
 * selected external node. Returns NULL if no suitable external node is found.
 */
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

/**
 * @brief Join a network for debugging purposes
 *
 * This function allows a host to join a network for debugging purposes. The function
 * receives a buffer containing the command and parameters for the join, a pointer to
 * the host joining the network, and a flag indicating if the join call was made
 * directly by the user or from a function call. The function performs several checks
 * on the input parameters, and if all of them are valid, it proceeds with the join
 * operation by inserting a new node in the network and exchanging messages between
 * the host and the external node.
 *
 * @param buffer: A pointer to a character array containing the command and
 * parameters for the join
 * @param host: a pointer to a host structure representing the host joining the
 * network
 * @param flag: an integer flag indicating if the join call was made directly by the
 * user or from a function call
 * @return 1 if the operation was successful, 0 otherwise
 */
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
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  if (sscanf(buffer, "djoin %s %s %s %s %s", net, ID, node_ID, node_IP, node_TCP) <
      5) {
    /*! TODO: Treat error: invalid user input or function failure */
    return 0;
  }

  /* If the input comes from join(), all arguments passed to the buffer are
   * properly checked and ready to be used. A direct call from the user to djoin()
   * must have all it's arguments verified */
  if (flag == DJOIN) {
    /*! TODO: Verify input buffer */
    if (check_net_and_id(net, ID) == EXIT_FAILURE ||
        check_node_parameters(node_ID, node_IP, node_TCP) == EXIT_FAILURE) {
      /*error*/ printf("Invalid djoin call\n");
      return 0;
    }
  }

  /* Single node in the network */
  if (strcmp(ID, node_ID) == 0) {
    assign_ID_and_net(host, ID, net);
    return 1;
  }

  insert_node(node_ID, -1, node_IP, atoi(node_TCP), host);
  assign_ID_and_net(host, ID, net);

  /* Message exchange between the host and the extern node */
  memset(msg_to_send, 0, sizeof(msg_to_send));
  sprintf(msg_to_send, "NEW %s %s %d\n", host->ID, host->uip->IP, host->uip->TCP);
  received_msg = fetch_bck(host, msg_to_send);
  if (received_msg == NULL) {
    leave_network(host, flag == JOIN ? JOIN : DJOIN);
    return 0; /* error */
  }

  sscanf(received_msg, "EXTERN %s %s %s", node_ID, node_IP, node_TCP);
  if (strcmp(node_ID, host->ID) != 0) {
    host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
  }

  return 1;
}

/**
 * @brief Join a network given its name and a unique ID
 *
 * First, it checks the user input for errors. Then, it sends a message to the node
 * server requesting the list of available nodes. After receiving the list, it sends
 * a message to the node server requesting to register the host. If registration is
 * successful, it checks if there are any external nodes in the network and, if so,
 * connects to one of them. If registration is not successful, it prints an error
 * message to the console.
 *
 * @param buffer: input buffer with the user command and arguments
 * @param host: pointer to the `host` struct
 * @return 1 if the host successfully joined the network, 0 otherwise
 */
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
      djoin_network(msg_to_send, host,
                    JOIN); // connects to the ext node in the network
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
