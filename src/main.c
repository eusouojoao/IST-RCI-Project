#include "common/parser.h"
#include "common/utils.h"
#include "error_handling/error_messages.h"
#include "interface/user_interface.h"
#include "protocols/TCP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define max(A, B) ((A) >= (B) ? (A) : (B))
#define SIZE 256

int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL)); // seed the rand function
  /* User arguments */
  user_args *uip = NULL;
  uip = parser(argc, argv);

  host *host = init_host(uip);

  /* File descriptors e counter do select() */
  int listen_fd = create_listen_socket(uip);
  int newfd = -1, maxfd = listen_fd, counter = 0;
  /* Inicializar o master fd_set */
  fd_set master_set, working_set;
  FD_ZERO(&master_set);
  FD_SET(listen_fd, &master_set);
  FD_SET(STDIN_FILENO, &master_set);

  /* Working buffer */
  char buffer[SIZE];
  memset(&buffer, 0, SIZE);

  struct sockaddr in_addr;
  socklen_t in_addrlen;

  /* Inicializar a estrutura timeval para o timeout */
  struct timeval timeout;
  timeout.tv_sec = 300; // 5min = 5 * 60s = 300s
  timeout.tv_usec = 0;

  CLEAR_STDIN();
  printf(BLUE "\tUser interface [" GREEN "ON" BLUE "]\n" RESET);
  for (/* for */; /* ever */; /* ! */) {
    printf(GREEN "<USER> " RESET);
    fflush(stdout);
    memcpy(&working_set, &master_set, sizeof(master_set));
    maxfd = max(listen_fd, maxfd);
    counter = select(maxfd + 1, &working_set, (fd_set *)NULL, (fd_set *)NULL,
                     (struct timeval *)&timeout);
    if (counter <= 0) {
      system_error("In main() ->" RED " select() failed");
      /*error*/ exit(EXIT_FAILURE);
    }

    for (/* X */; counter > 0; counter--) {
      if (FD_ISSET(STDIN_FILENO, &working_set)) {
        memset(&buffer, 0, SIZE);
        if (read(STDIN_FILENO, buffer, SIZE) == -1) {
          system_error("In main() ->" RED " read() failed");
          /*error*/ exit(EXIT_FAILURE);
        }
        /* Ver se funciona, remover mais tarde */
        printf("buffer: %s\n", buffer);

        process_stdin_input(buffer, host);
        FD_CLR(STDIN_FILENO, &working_set);
      }

      else if (FD_ISSET(listen_fd, &working_set)) {
        if ((newfd = accept(listen_fd, &in_addr, &in_addrlen)) == -1) {
          system_error("In main() ->" RED " accept() failed");
          /*error*/ exit(EXIT_FAILURE);
        }

        if (read(newfd, buffer, SIZE) == -1) {
          system_error("In main() ->" RED " read() failed");
          /*error*/ exit(EXIT_FAILURE);
        }

        /*! TODO: Ver mais tarde
         *
         * @todo Guardar o file descriptor da ligação criada com o nó interno na
         * lista de nós vizinhos, e interagir com a mensagem recebida.
         */

        FD_CLR(listen_fd, &working_set);
      }

      else if (/* FD_ISSET(fd_guardado, &working_set) */ 0) {
        ;

        /*! TODO: Ver mais tarde
         *
         * @todo Tratamento da mensagem de uma socket de um vizinhos (interno e
         * externo). Primeiro ver como iterar sobre os vizinhos ativos no array
         * de flags working_set
         */

        // FD_CLR(fd_guardado, &working_set);
      }
    }
  }

  return EXIT_SUCCESS;
}
