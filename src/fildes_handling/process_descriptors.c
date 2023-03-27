#include "process_descriptors.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "../essentials/circular_buffer.h"
#include "../essentials/host_handling.h"
#include "../essentials/new_connections_list.h"
#include "custom_protocols_interface/utility.h"
#include "user_interface/user_commands.h"
#include "user_interface/utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 32

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
      {"join", JOIN},        {"djoin", DJOIN},     {"leave", LEAVE},
      {"exit", EXIT},        {"create", CREATE},   {"delete", DELETE},
      {"get", GET},          {"show", SHOW},       {"st", SHOW_TOPOLOGY},
      {"sn", SHOW_NAMES},    {"sr", SHOW_ROUTING}, {"clear", CLEAR},
      {"cr", CLEAR_ROUTING}, {"cn", CLEAR_NAMES},  {"cw", CLEAR_WINDOW},
      {"help", HELP},        {"?", HELP}};

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
  static user_command flag = UNDEF;
  user_command cmd = UNDEF;
  char token[32] = {'\0'};
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("sscanf() failed");
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
    if (djoin_network(buffer, host, (int)cmd)) {
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
    get_name(host, buffer);
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
  case CLEAR_WINDOW:
  case CLEAR_NAMES:
  case CLEAR_ROUTING:
    clear_wrapper(host, cmd, buffer);
    break;
  case HELP:
    print_help();
    break;
  case UNDEF:
  default:
    user_input_error("Command Invalid", buffer, "Here are the valid user commands:");
    print_help();
    return -1;
  }

  return 1;
}

// APAGAR - refazer este comment
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
 * @param connection: a pointer to a new_connection structure
 */
void process_new_connection(host *host, new_connection *connection) {
  char buffer[SIZE << 2] = {'\0'}, msg_to_send[SIZE << 2] = {'\0'};
  char new_ID[SIZE] = {'\0'}, new_IP[SIZE] = {'\0'}, new_TCP[SIZE] = {'\0'};

  // Read from the connection's circular buffer until a newline character is found
  if (!cb_read_LF(connection->cb, buffer, sizeof(buffer) - 1)) {
    return;
  }

  // Parse the "NEW" command from the received message
  if (sscanf(buffer, "NEW %s %s %s\n", new_ID, new_IP, new_TCP) != 3) {
    close(connection->new_fd);
    remove_new_connection(host, connection->new_fd);
    return;
  }

  // Validate the parsed node parameters
  if (check_node_parameters(new_ID, new_IP, new_TCP) == EXIT_FAILURE) {
    close(connection->new_fd);
    remove_new_connection(host, connection->new_fd);
    return;
  }

  // Send the "EXTERN" message to the new node
  // If the host is an anchor node, use the new node's information
  // Otherwise, use the external node's information
  if (host->ext == NULL) { // Anchor node case
    sprintf(msg_to_send, "EXTERN %s %s %s\n", new_ID, new_IP, new_TCP);
  } else { // Normal case
    node *ext = host->ext;
    sprintf(msg_to_send, "EXTERN %s %s %d\n", ext->ID, ext->IP, ext->TCP);
  }

  if (write(connection->new_fd, msg_to_send, strlen(msg_to_send)) == -1) {
    close(connection->new_fd);
    remove_new_connection(host, connection->new_fd);
    return;
  }

  // Insert the new node into the host's node list
  insert_node(new_ID, connection->new_fd, new_IP, atoi(new_TCP), host);

  // Update the forwarding table
  insert_in_forwarding_table(host, atoi(new_ID), atoi(new_ID));

  // Remove the new connection from the new_connections_list
  remove_new_connection(host, connection->new_fd);
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
  protocol_command cmd = BADFORMAT;
  char token[SIZE] = {'\0'};

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
    process_query(host, node, buffer);
    break;
  case CONTENT:
  case NOCONTENT:
    handle_content_response(host, node, buffer, cmd);
    break;
  case BADFORMAT:
  default:
    /*error*/
    fprintf(stderr, "Something something bad format from NODE: %s\n", node->ID);
    break;
  }
}
