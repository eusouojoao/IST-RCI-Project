#include "error_checking.h"
#include "error_messages.h"

#include <arpa/inet.h>
#include <ctype.h>
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
 * @return 1 if the parameters are correct, 0 otherwise.
 */
int check_node_parameters(char *node_ID, char *node_IP, char *node_TCP) {
  if (!(check_id(node_ID) && check_PORT(node_TCP) && check_IP_address(node_IP))) {
    return 0;
  }

  return 1;
}

/**
 * @brief Checks if the network parameter is in the correct format.
 *
 * @param net: the network parameter
 * @return 1 if the network parameter is invalid, 0 otherwise
 */
int check_net(char *net) {
  if (strlen(net) != 3 || !check_if_number(net)) {
    user_input_error("Invalid network", net, "net must be a number between 000 and 999");
    return 0;
  }

  int int_net = atoi(net);
  if (int_net < 0 || int_net > 999) {
    user_input_error("Invalid network", net, "net must be a number between 000 and 999");
    return 0;
  }

  return 1; // Value within range
}

/**
 * @brief Checks if the ID parameter is in the correct format.
 *
 * @param id: the ID parameter
 * @return 1 if the ID parameter is valid, 0 otherwise
 */
int check_id(char *id) {
  if (strlen(id) != 2 || !check_if_number(id)) {
    user_input_error("Invalid ID", id, "ID must be a number between 00 and 99");
    return 0;
  }

  int int_id = atoi(id);
  if (int_id < 0 || int_id > 99) {
    user_input_error("Invalid ID", id, "ID must be a number between 00 and 99");
    return 0;
  }

  return 1; // Value within range
}

/**
 * @brief Checks if the network and ID parameters are in the correct format.
 *
 * @param net: the network parameter
 * @param id: the ID parameter
 * @return 1 if successful, 0 otherwise
 */
int check_net_and_id(char *net, char *id) { return check_net(net) && check_id(id); }

/**
 * @brief Check if the source string only has numbers
 *
 * @param src: string to be checked
 * @return 1 if successful, 0 otherwise
 **/
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
 * @return the port number as an integer if the format is correct, -1 otherwise.
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
 * @return 1 if the IP address is in the correct format, 0 otherwise.
 */
int check_IP_address(char *src) {
  struct sockaddr_in sa;
  int retval = inet_pton(AF_INET, src, &(sa.sin_addr));
  if (!retval) {
    user_input_error("Invalid IPv4", src,
                     "the IP format must be X.X.X.X, where X must be a "
                     "decimal value between 0 and 255 (octet).");
  }
  return retval;
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
        "Name must be alphanumeric('.' is an exception) and at most 100 caracter long.");
    return -1;
  }

  for (size_t i = 0; i < len; i++) {
    if (!isalnum(name[i]) && name[i] != '.') {
      user_input_error("Invalid name", name,
                       "Name must be alphanumeric ('.' is an exception) and at most 100 "
                       "caracter long.");
      return -1;
    }
  }

  return 0;
}
