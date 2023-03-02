#include "../hdr/parser.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535

/* Inicializa uma estrutura do tipo user_input alocada
 * dinamicamente (e passada por referência), com o intuito
 * de proceder à avaliação dos parâmetros introduzidos
 * pelo utilizador (será overwritten futuramente, caso
 * os parâmetros passem o estágio de avaliação subsequente) */
void init_uip(user_args **uip) {
  (*uip)->IP = "NULL";
  (*uip)->TCP = 0;
  (*uip)->regIP = "193.136.138.142";
  (*uip)->regUDP = 59000;

  return;
}

int check_PORT(char *src) {
  char *end = NULL;
  long PORT = strtol(src, &end, 10);
  if ((end == src) || (*end != '\0')) {
    exit(EXIT_FAILURE);
  }

  if (PORT < 0 || PORT > MAXPORT) {
    /*out of range*/ exit(EXIT_FAILURE);
  }

  return (int)PORT;
}

int check_IP_address(char *src) {
  struct sockaddr_in sa;
  return inet_pton(AF_INET, src, &(sa.sin_addr));
}

int check_input_integrity(int argc, char *argv[], user_args **uip) {

  if (check_IP_address(argv[1]) != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }
  (*uip)->IP = argv[1];              // Atribui o IP validado
  (*uip)->TCP = check_PORT(argv[2]); // Atribui a porta validada

  if (argc == 5) {
    if (check_IP_address(argv[3]) != 1) {
      /*error*/ exit(EXIT_FAILURE);
    }
    (*uip)->regIP = argv[3];              // Atribui o IP validado
    (*uip)->regUDP = check_PORT(argv[4]); // Atribui a porta validade
  }

  return EXIT_SUCCESS;
}

user_args *parser(int argc, char *argv[]) {
  int input_ok = 0;

  /* Verifica se o número mínimo ou máximo de parâmetros
   * introduzidos pelo utilizador na invocação do programa
   * viola as especificações enunciadas */
  if (argc != 3 && argc != 5) {
    /*error*/ exit(EXIT_FAILURE);
  }

  /* Inicialização da estrutura do tipo user_input */
  user_args *uip = calloc(1, sizeof(user_args));
  if (uip == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  init_uip(&uip);

  /* Verificar a integridade dos parâmetros introduzidos pelo
   * utilizador na invocação do programa: IP TCP (regIP regTCP)
   * e atribui, caso se verifique, os valores à estrutura user_input */
  input_ok = check_input_integrity(argc, argv, &uip);
  if (input_ok != 0) {
    /*error*/ exit(EXIT_FAILURE);
  }

  return uip;
}
