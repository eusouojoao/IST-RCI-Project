#include "../hdr/TCP_utils.h"
#include "../hdr/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define max(A, B) ((A) >= (B) ? (A) : (B))
#define clear() printf("\033[H\033[J")
#define BUFFER_SIZE 128

int main(int argc, char *argv[]) {
  user_args *uip = NULL;
  uip = parser(argc, argv);
  fd_set readfds;
  int fd = create_listen_socket(uip);

  int newfd = -1, maxfd = -1; // counter = 0;
  char buffer[BUFFER_SIZE] = "";
  struct sockaddr incomming_addr;
  socklen_t incomming_addrlen;

  clear();
  for (;;) {
    fflush(stdout);
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(fd, &readfds);
    maxfd = fd;
    select(maxfd + 1, &readfds, (fd_set *)NULL, (fd_set *)NULL,
           (struct timeval *)NULL);

    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      if (read(STDIN_FILENO, buffer, BUFFER_SIZE) == -1) {
        /*error*/ exit(EXIT_FAILURE);
      }
      /* Ver se funciona, remover mais tarde */
      printf("Ler do teclado -> buffer: %s\n", buffer);
    }

    if (FD_ISSET(fd, &readfds)) {
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
    }
  }

  free(uip);
  return EXIT_SUCCESS;
}
