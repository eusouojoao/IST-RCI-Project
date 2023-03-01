#include "../hdr/parser.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inicializa uma estrutura do tipo user_input alocada
 * dinamicamente (e passada por referência), com o intuito
 * de proceder à avaliação dos parâmetros introduzidos
 * pelo utilizador (será overwritten futuramente, caso
 * os parâmetros passem o estágio de avaliação subsequente) */
void init_uip(user_input **uip) {
  (*uip)->IP = "NULL";
  (*uip)->TCP = 0;
  (*uip)->regIP = "193.136.138.142";
  (*uip)->regTCP = 59000;

  return;
}

int check_input_integrity(int argc, char *argv[], user_input **uip) {
  int PORT;

  if (inet_pton(AF_INET, argv[2], (struct sockaddr_in *)NULL) != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  if ((PORT = atoi(argv[2])) < 0 || PORT > 65535) {
    /*error*/ exit(EXIT_FAILURE);
  }

  if (argc == 5) {
    if (inet_pton(AF_INET, argv[4], (struct sockaddr_in *)NULL) != 1) {
      /*error*/ exit(EXIT_FAILURE);
    }

    if ((PORT = atoi(argv[5])) < 0 || PORT > 65535) {
      /*error*/ exit(EXIT_FAILURE);
    }
  }

  return EXIT_SUCCESS;
}

user_input *parser(int argc, char *argv[]) {
  int input_ok = 0;
  printf("argc = %d\n\n", argc);

  /* Verifica se o número mínimo ou máximo de parâmetros
   * introduzidos pelo utilizador na invocação do programa
   * viola as especificações enunciadas */
  if (argc != 3 && argc != 5) {
    /*error*/ exit(EXIT_FAILURE);
  }

  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("\n");

  /* Inicialização da estrutura do tipo user_input */
  user_input *uip = calloc(1, sizeof(user_input));
  if (uip == NULL) {
    /*error*/ exit(EXIT_FAILURE);
  }
  init_uip(&uip);

  printf("%s\n", uip->IP);
  printf("%d\n", uip->TCP);
  printf("%s\n", uip->regIP);
  printf("%d\n", uip->regTCP);

  /* Verificar a integridade dos parâmetros introduzidos pelo
   * utilizador na invocação do programa: IP TCP (regIP regTCP)
   * e atribui, caso se verifique, os valores à estrutura user_input */
  input_ok = check_input_integrity(argc, argv, &uip);
  if (input_ok != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  return uip;
}
