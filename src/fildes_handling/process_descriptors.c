#include "process_descriptors.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "socket_protocols_interface/utility.h"
#include "user_interface/utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

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

void process_stdin_input(host *host, char *buffer) {
  static int flag = -1;
  char token[32] = {'\0'};
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_stdin_input() ->" RED " sscanf() failed");
    return;
  }

  switch (get_user_command(token)) {
  case JOIN:
    if (join_network(buffer, host)) {
      flag = JOIN;
    }
    break;
  case DJOIN:
    if (djoin_network(buffer, host, DJOIN)) {
      flag = DJOIN;
    }
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
    leave_network(host, flag);
    break;
  case EXIT:
    exit_program(host, flag);
    break;
  case CLEAR:
    CLEAR_STDIN();
    printf(BLUE "%*s User interface [" GREEN "ON" BLUE "]\n" RESET, 6, "");
    break;
  case UNDEF:
  default:
    /*error*/ // exit(EXIT_FAILURE);
    break;
  }

  return;
}

/*
 * @brief Process a new file descriptor for the host
 *
 * This function processes a new file descriptor received by the host. The function
 * first parses the message contained in the buffer and checks that it is a "NEW"
 * command. If the message is invalid or the command is not "NEW", the function
 * closes the file descriptor and returns. If the message is valid, the function
 * sends an "EXTERN" message to the new node and inserts the new node in the
 * host's node list.
 *
 * @param host: a pointer to the host structure
 * @param new_fd: the new file descriptor
 * @param buffer: a buffer containing the message to process
 */
void process_new_fd(host *host, int new_fd, char *buffer) {
  char msg_to_send[SIZE << 2] = {'\0'}, cmd[SIZE] = {'\0'};
  char new_ID[SIZE] = {'\0'}, new_IP[SIZE] = {'\0'}, new_TCP[SIZE] = {'\0'};

  if (sscanf(buffer, "%s %s %s %s", cmd, new_ID, new_IP, new_TCP) < 4) {
    /*error*/
    close(new_fd);
    return;
  }
  if (strcmp(cmd, "NEW") != 0) {
    /*error*/
    perror("Invalid command");
    close(new_fd);
    return;
  }
  if (check_node_parameters(new_ID, new_IP, new_TCP) == EXIT_FAILURE) {
    /*error*/
    close(new_fd);
    return;
  }

  if (host->ext == NULL) { // anchor node case
    sprintf(msg_to_send, "EXTERN %s %s %s\n", new_ID, new_IP, new_TCP);
    if (write(new_fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      /*error*/;
      close(new_fd);
      return;
    }

    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  } else { // normal case
    sprintf(msg_to_send, "EXTERN %s %s %d\n", host->ext->ID, host->ext->IP,
            host->ext->TCP);
    if (write(new_fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      /*error*/;
      close(new_fd);
      return;
    }

    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  }

  return; // OK
}

/**
 * @brief Returns the corresponding protocol command for a given token
 *
 * @param token: the token to be checked
 * @return the protocol command corresponding to the token
 */
protocol_command get_protocol_command(char *token) {
  if (strcmp(token, "EXTERN") == 0)
    return EXTERN;
  if (strcmp(token, "WITHDRAW") == 0)
    return WITHDRAW;
  if (strcmp(token, "QUERY") == 0)
    return QUERY;
  if (strcmp(token, "CONTENT") == 0)
    return CONTENT;
  if (strcmp(token, "NOCONTENT") == 0)
    return NOCONTENT;

  return BADFORMAT;
}

/**
 * @brief Processes a message received from a neighbour node and performs the
 * corresponding action
 *
 * @param host: the host that received the message
 * @param node: the neighbour node that sent the message
 * @param buffer: the message received
 */
void process_neighbour_node_fd(host *host, node *node, char *buffer) {
  char token[32] = {'\0'}, ID[32] = {'\0'};
  printf("buffer: %s\n", buffer); // DEBUG

  // Get the first token from the message
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
    return;
  }

  // Check the protocol command and perform the corresponding action
  switch (get_protocol_command(token)) {
  case EXTERN:
    // If the command is EXTERN, update the backup
    update_backup(host, buffer);
    break;
  case WITHDRAW:
    // If the command is WITHDRAW, withdraw the node with the specified ID
    if (sscanf(buffer, "WITHDRAW %s", ID) < 1) {
      system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
      return;
    }
    if (!(strlen(ID) == 2 && check_if_number(ID))) {
      /* error, protocol with bad format */
      printf("Error! Bad format!\n");
      return;
    }
    break;
  case QUERY:
  case CONTENT:
  case NOCONTENT:
    break;
  case BADFORMAT:
  default:
    /*error*/
    printf("Something something bad format from NODE: %s\n", node->ID);
    break;
  }
}
