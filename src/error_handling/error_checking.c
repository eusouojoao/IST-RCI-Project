#include "error_checking.h"
#include "error_messages.h"

#include <arpa/inet.h>
#include <ctype.h>
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
    printf("strlen\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(node_ID) && check_if_number(node_TCP)) ||
      (check_IP_address(node_IP) != 1)) {
    printf("not a number\n");
    return EXIT_FAILURE;
  }

  int int_id = atoi(node_ID), int_tcp = atoi(node_TCP);
  if ((int_id < 0 || int_id > 99) || (int_tcp < 0 || int_tcp > MAXPORT)) {
    printf("out of range\n");
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
    /*error*/ printf("strlen\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(net) && check_if_number(id))) {
    /*error*/ printf("not a number\n");
    return EXIT_FAILURE;
  }

  int int_net = atoi(net), int_id = atoi(id);
  if ((int_net < 0 || int_net > 999) || (int_id < 0 || int_id > 99)) {
    /*error*/ printf("out of range\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; // Values within range
}

int check_if_number(char *src) {
  for (size_t i = 0; i < strlen(src); i++) {
    if (!isdigit(src[i])) {
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
 * @return The port number as an integer if the format is correct, EXIT_FAILURE otherwise.
 */
int check_PORT(char *src) {
  char *end = NULL;
  long PORT = strtol(src, &end, 10);
  if ((end == src) || (*end != '\0')) {
    user_input_error("Bad port format", src, "must be an integer within 0 and 65535.");
    exit(EXIT_FAILURE);
  }

  if (PORT < 0 || PORT > MAXPORT) {
    user_input_error("Port out of range", src, "does not lie within 0 and 65535.");
    exit(EXIT_FAILURE);
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
  struct sockaddr_in sa;
  return inet_pton(AF_INET, src, &(sa.sin_addr));
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
  if (strlen(name) > 100) {
    return -1;
  }

  for (size_t i = 0; i < strlen(name); i++) {
    if (!isalnum(name[i]) || name[i] != '.') {
      return -1;
    }
  }

  return 0;
}
