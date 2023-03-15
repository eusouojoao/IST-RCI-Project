#include "error_checking.h"
#include "error_messages.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535

int number_of_command_arguments(char *str, char delim) {
  int n = 0;
  for (int i = 0; str[i]; i++) {
    n += (str[i] == delim);
  }

  return n;
}

int check_node_parameters(char *node_ID, char *node_IP, char *node_TCP) {
  if (strlen(node_ID) != 2) {
    /*error*/ printf("strlen\n");
    return EXIT_FAILURE;
  }

  if (!(check_if_number(node_ID) && check_if_number(node_TCP)) ||
      (check_IP_address(node_IP) != 1)) {
    /*error*/ printf("not a number\n");
    return EXIT_FAILURE;
  }

  int int_id = atoi(node_ID), int_tcp = atoi(node_TCP);
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
  if ((int_net < 0 || int_net > 999) || (int_id < 0 || int_id > 99)) {
    /*error*/ printf("out of range\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS; // valores dentro do range
}

int check_if_number(char *src) {
  for (int i = 0; i < (int)strlen(src); i++) {
    if (!isdigit(src[i])) {
      return 0;
    }
  }
  return 1;
}

int check_PORT(char *src) {
  char *end = NULL;
  long PORT = strtol(src, &end, 10);
  if ((end == src) || (*end != '\0')) {
    user_input_error("Bad port format", src,
                     "must be an integer within 0 and 65535.");
    exit(EXIT_FAILURE);
  }

  if (PORT < 0 || PORT > MAXPORT) {
    user_input_error("Port out of range", src, "does not lie within 0 and 65535.");
    exit(EXIT_FAILURE);
  }

  return (int)PORT;
}

int check_IP_address(char *src) {
  struct sockaddr_in sa;
  return inet_pton(AF_INET, src, &(sa.sin_addr));
}

/**
 * @brief  Verifica se um nome cumpre as restrições (menos de 100 caracteres)
 *
 * @note
 * @param  *name: nome a ser verificafo
 * @retval 0 nome válido
 *         -1 nome inválido
 */
int check_name(char *name) {
  if (strlen(name) > 100) {
    return -1;
  }

  for (size_t i = 0; i < strlen(name); i++) {
    if (!isalnum(name[i])) {
      return -1;
    }
  }

  return 0;
}
