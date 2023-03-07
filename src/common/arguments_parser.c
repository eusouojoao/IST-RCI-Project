#include "arguments_parser.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"

#include <stdlib.h>

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

int check_input_integrity(int argc, char *argv[], user_args **uip) {

  if (check_IP_address(argv[1]) != 1) {
    user_input_error("Invalid IPv4", argv[1],
                     "the IP format must be X.X.X.X, where X must be a "
                     "decimal value between 0 and 255 (octet).");
  }
  (*uip)->IP = argv[1];              // Atribui o IP validado
  (*uip)->TCP = check_PORT(argv[2]); // Atribui a porta validada

  if (argc == 5) {
    if (check_IP_address(argv[3]) != 1) {
      user_input_error("Invalid IPv4", argv[3],
                       "the IP format must be X.X.X.X, where X must be a "
                       "decimal value between 0 and 255 (octet).");
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
    /*error*/ usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Inicialização da estrutura do tipo user_input */
  user_args *uip = calloc(1, sizeof(user_args));
  if (uip == NULL) {
    /*error*/ system_error("In parser() -> calloc() failed: ");
    exit(EXIT_FAILURE);
  }
  init_uip(&uip);

  /* Verificar a integridade dos parâmetros introduzidos pelo
   * utilizador na invocação do programa: IP TCP (regIP regTCP)
   * e atribui, caso se verifique, os valores à estrutura user_input */
  input_ok = check_input_integrity(argc, argv, &uip);
  if (input_ok != EXIT_SUCCESS) {
    /*error*/ usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  return uip;
}
