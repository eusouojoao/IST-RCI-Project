#include "error_checking.h"
#include "error_messages.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535

/**
 * @brief Counts the number of command arguments in a string.
 *
 * @param str: input string to count arguments
 * @param delim: delimiter character used to separate arguments
 *
 * @return the number of command arguments in the input string.
 */
int number_of_command_arguments(char *str, char delim) {
  int n = 0;
  for (int i = 0; str[i]; i++) {
    n += (str[i] == delim);
  }
  return n;
}

/**
 * @brief Checks if the node parameters are in the correct format.
 *
 * @param node_ID: the node ID
 * @param node_IP: the node IP address
 * @param node_TCP: the node TCP port number
 *
 * @return EXIT_SUCCESS if the parameters are correct, EXIT_FAILURE otherwise.
 */
int check_node_parameters(char *node_ID, char *node_IP, char *node_TCP) {
  if (strlen(node_ID) != 2) {
    user_input_error("Invalid ID", node_ID, "ID must be a number between 00 and 99");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(node_ID) && check_if_number(node_TCP)) ||
      (check_IP_address(node_IP) != 1)) {
    // ID, IP, TCP invalid
    return EXIT_FAILURE;
  }

  int int_id = atoi(node_ID), int_tcp = atoi(node_TCP);
  if ((int_id < 0 || int_id > 99)) {
    user_input_error("Invalid ID", node_ID, "ID must be a number between 00 and 99");
    return EXIT_FAILURE;
  }
  if ((int_tcp < 0 || int_tcp > MAXPORT)) {
    user_input_error("Invalid TCP", node_TCP, "TCP must be a number between 0 and 65534");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/**
 * @brief Checks if the network and ID parameters are in the correct format.
 *
 * @param net: the network parameter
 * @param id: the ID parameter
 *
 * @return EXIT_SUCCESS if the parameters are correct, EXIT_FAILURE otherwise.
 */
int check_net_and_id(char *net, char *id) {
  if (strlen(net) != 3 || strlen(id) != 2) {
    if (strlen(id) != 2)
      user_input_error("Invalid ID", id, "ID must be a number between 00 and 99");
    if (strlen(net) != 3)
      user_input_error("Invalid net", net, "net must be a number between 000 and 999");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(net) && check_if_number(id))) {
    /*error*/
    return EXIT_FAILURE;
  }

  int int_net = atoi(net), int_id = atoi(id);
  if ((int_net < 0 || int_net > 999)) {
    user_input_error("Invalid net", net, "net must be a number between 000 and 999");
    return EXIT_FAILURE;
  }
  if ((int_id < 0 || int_id > 99)) {
    user_input_error("Invalid ID", id, "ID must be a number between 00 and 99");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; // Values within range
}

int check_if_number(char *src) {
  for (size_t i = 0; i < strlen(src); i++) {
    if (!isdigit(src[i])) {
      user_input_error("Value not a number", src, "Inputs like ID, net, TCP must be numbers");
      return 0;
    }
  }
  return 1;
}

/**
 * @brief Checks if the given port is in the correct format and within range.
 *
 * @param src: the input port string to check
 *
 * @return The port number as an integer if the format is correct, -1 otherwise.
 */
int check_PORT(char *src) {
  char *end = NULL;
  long PORT = strtol(src, &end, 10);
  if ((end == src) || (*end != '\0')) {
    user_input_error("Bad port format", src, "must be an integer within 0 and 65535.");
    return -1;
  }

  if (PORT < 0 || PORT > MAXPORT) {
    user_input_error("Port out of range", src, "does not lie within 0 and 65535.");
    return -1;
  }

  return (int)PORT;
}

/**
 * @brief Checks if the given IP address is in the correct format.
 *
 * @param src: the input IP address string to check
 *
 * @return 1 if the IP address is in the correct format, 0 otherwise.
 */
int check_IP_address(char *src) {
  int r;
  struct sockaddr_in sa;
  if (!(r = inet_pton(AF_INET, src, &(sa.sin_addr)))) {
    user_input_error("Invalid IPv4", src,
                     "the IP format must be X.X.X.X, where X must be a "
                     "decimal value between 0 and 255 (octet).");
  }
  return r;
}

/**
 * @brief Checks if a name meets the constraints (less than 100 characters and
 * alphanumeric).
 *
 * @param name: the name to be checked
 *
 * @return 0 if the name is valid, -1 otherwise.
 */
int check_name(char *name) {
  size_t len = strlen(name);
  if (len <= 0 || len > 100) {
    user_input_error(
        "Invalid name", name,
        "Name must be alphanumeric('.' is an exception) and at most 100 caracter long");
    return -1;
  }

  for (size_t i = 0; i < len; i++) {
    if (!isalnum(name[i]) && name[i] != '.') {
      user_input_error("Invalid name", name,
                       "Name must be alphanumeric ('.' is an exception) and at most 100 "
                       "caracter long");
      return -1;
    }
  }

  return 0;
}
