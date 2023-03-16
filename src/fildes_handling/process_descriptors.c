#include "process_descriptors.h"
#include "../common/prompts.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "socket_protocols_interface/protocol_commands.h"
#include "socket_protocols_interface/utility.h"
#include "socket_protocols_interface/withdraw_module.h"
#include "user_interface/content_module.h"
#include "user_interface/user_commands.h"
#include "user_interface/utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

typedef struct {
  char *token;
  user_command command;
} token_command_pair;

typedef struct {
  char *token;
  protocol_command protocol;
} token_protocol_pair;

user_command get_user_command(char *token) {
  // Static array of struct pairs that maps token strings to user commands
  static const token_command_pair command_lookup[] = {
      {"join", JOIN},        {"djoin", DJOIN},   {"create", CREATE},
      {"delete", DELETE},    {"get", GET},       {"show", SHOW},
      {"st", SHOW_TOPOLOGY}, {"sn", SHOW_NAMES}, {"sr", SHOW_ROUTING},
      {"leave", LEAVE},      {"exit", EXIT},     {"clear", CLEAR},
  };

  // Calculate the number of elements in the command_lookup array
  size_t number_of_elements = sizeof(command_lookup) / sizeof(token_command_pair);

  // Iterate through the command_lookup array, looking for a pair whose token field
  // matches the input token string
  for (size_t i = 0; i < number_of_elements; i++) {
    if (strcmp(token, command_lookup[i].token) == 0) {
      // If a match is found, return the corresponding command field
      return command_lookup[i].command;
    }
  }

  return UNDEF;
}

int process_keyboard_input(host *host, char *buffer) {
  static int flag = -1;
  int cmd = UNDEF;
  char token[32] = {'\0'};
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_stdin_input() ->" RED " sscanf() failed");
    return -1;
  }

  cmd = get_user_command(token);
  switch (cmd) {
  case JOIN:
    if (join_network(buffer, host)) {
      flag = cmd;
    }
    break;
  case DJOIN:
    if (djoin_network(buffer, host, cmd)) {
      flag = cmd;
    }
    break;
  case CREATE:;
    insert_name(host, buffer);
    break;
  case DELETE:
    delete_name(host, buffer);
    break;
  case GET:
    /*! TODO: */
    break;
  case SHOW:
  case SHOW_TOPOLOGY:
  case SHOW_NAMES:
  case SHOW_ROUTING:
    show_wrapper(host, cmd, buffer);
    break;
  case LEAVE:
    leave_network(host, flag);
    break;
  case EXIT:
    return exit_program(host, flag);
  case CLEAR:
    CLEAR_STREAM(STDIN_FILENO);
    user_interface_toggle(ON);
    break;
  case UNDEF:
  default:
    return -1; // Error, shouldn't be here
    break;
  }

  return 1;
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
void process_new_connection(host *host, int new_fd, char *buffer) {
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

  insert_in_forwarding_table(host, atoi(new_ID), atoi(new_ID));
  return; // OK
}

/**
 * @brief Returns the corresponding protocol command for a given token
 *
 * @param token: the token to be checked
 *
 * @return the protocol command corresponding to the token
 */
protocol_command get_protocol_command(char *token) {
  //  Static array of struct pairs that maps token strings to protocol commands
  static const token_protocol_pair protocol_lookup[] = {
      {"EXTERN", EXTERN},   {"WITHDRAW", WITHDRAW},   {"QUERY", QUERY},
      {"CONTENT", CONTENT}, {"NOCONTENT", NOCONTENT},
  };

  // Calculate the number of elements in the protocol_lookup array
  size_t number_of_elements = sizeof(protocol_lookup) / sizeof(token_protocol_pair);

  // Iterate through the protocol_lookup array, looking for a pair whose token field
  // matches the input token string
  for (size_t i = 0; i < number_of_elements; i++) {
    if (strcmp(token, protocol_lookup[i].token) == 0) {
      // If a match is found, return the corresponding command field
      return protocol_lookup[i].protocol;
    }
  }

  // If no match is found, return BADFORMAT
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
void process_neighbour_nodes(host *host, node *node, char *buffer) {
  int cmd = BADFORMAT;
  char token[32] = {'\0'};
  printf("buffer: %s\n", buffer); // DEBUG

  // Get the first token from the message
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
    return;
  }

  // Check the protocol command and perform the corresponding action
  cmd = get_protocol_command(token);
  switch (cmd) {
  case EXTERN:
    // If the command is EXTERN, update the backup
    update_backup(host, buffer);
    break;
  case WITHDRAW:
    withdraw_wrapper(host, node, buffer);
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
