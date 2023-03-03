#include "../hdr/TCP_utils.h"
#include "../hdr/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define max(A, B) ((A) >= (B) ? (A) : (B))
#define clear() printf("\033[H\033[J")
#define BUFFER_SIZE 128

#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

int main(int argc, char *argv[]) {
  user_args *uip = NULL;
  uip = parser(argc, argv);
  fd_set readfds;
  int fd = create_listen_socket(uip);

  int newfd = -1, maxfd = -1, counter = 0;
  char buffer[BUFFER_SIZE] = "";
  struct sockaddr incomming_addr;
  socklen_t incomming_addrlen;

  clear();
  for (;;) {
    printf(GREEN "<USER> " RESET);
    fflush(stdout);
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(fd, &readfds);
    maxfd = fd;
    counter = select(maxfd + 1, &readfds, (fd_set *)NULL, (fd_set *)NULL,
                     (struct timeval *)NULL);
    if (counter <= 0) {
      /*error*/ exit(EXIT_FAILURE);
    }

    for (/*  */; counter > 0; counter--) {
      if (FD_ISSET(STDIN_FILENO, &readfds)) {
        memset(&buffer, 0, BUFFER_SIZE);
        if (read(STDIN_FILENO, buffer, BUFFER_SIZE) == -1) {
          /*error*/ exit(EXIT_FAILURE);
        }
        /* Ver se funciona, remover mais tarde */
        printf("buffer: %s\n", buffer);

        /*! TODO: Implementar parser para o input de teclado
         *
         * @todo parser que verifica o input do teclado e chama as funcões de
         * acordo com o pedido pelo utilizador, ou processa o erro
         */
        FD_CLR(STDIN_FILENO, &readfds);
      }

      else if (FD_ISSET(fd, &readfds)) {
        if ((newfd = accept(fd, &incomming_addr, &incomming_addrlen)) == -1) {
          /*error*/ exit(EXIT_FAILURE);
        }

        if (read(newfd, buffer, BUFFER_SIZE) == -1) {
          /*error*/ exit(EXIT_FAILURE);
        }

        /*! TODO: Ver mais tarde
         *
         * @todo Guardar o file descriptor da ligação criada com o nó interno na
         * lista de nós vizinhos, e interagir com a mensagem recebida.
         */
        FD_CLR(fd, &readfds);
      }

      else if (/* FD_ISSET(fd_guardado, &readfds) */ 0) {
        ;
        /*! TODO: Ver mais tarde
         *
         * @todo Tratamento da mensagem de uma socket de um vizinhos (interno e
         * externo). Primeiro ver como iterar sobre os vizinhos ativos no array
         * de flags readfds
         */
        // FD_CLR(fd_guardado, &readfds);
      }
    }
  }

  return EXIT_SUCCESS;
}
