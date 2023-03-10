#include "common/arguments_parser.h"
#include "common/utils.h"
#include "descriptor_utils/descriptor_control.h"
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

#define SIZE 256

int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL)); // seed the rand function
  /* User arguments */
  user_args *uip = NULL;
  uip = parser(argc, argv);

  /* Inicializar a estrutura do host e temp */
  host *host = init_host(uip);
  node *temp = NULL;

  /* File descriptors e counter do select() */
  host->listen_fd = create_listen_socket(uip);
  int new_fd = -1, counter = 0;

  /* Inicializar o master fd_set */
  fd_set master_set, working_set;
  FD_ZERO(&master_set);
  FD_SET(host->listen_fd, &master_set);
  FD_SET(STDIN_FILENO, &master_set);

  /* Working buffer */
  char buffer[SIZE];
  memset(&buffer, 0, SIZE);

  struct sockaddr in_addr;
  socklen_t in_addrlen = sizeof(in_addr);

  /* Inicializar a estrutura timeval para o timeout */
  struct timeval timeout;
  timeout.tv_sec = 300; // 5min = 5 * 60s = 300s
  timeout.tv_usec = 0;

  CLEAR_STDIN();
  printf(BLUE "%*s User interface [" GREEN "ON" BLUE "]\n" RESET, 6, "");
  for (/* for */; /* ever */; /* ! */) {
    printf(GREEN "<USER> " RESET);
    fflush(stdout);
    memcpy(&working_set, &master_set, sizeof(master_set));
    for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
      FD_SET(temp->fd, &working_set);
    }

    counter = select(get_maxfd(host) + 1, &working_set, (fd_set *)NULL, (fd_set *)NULL,
                     (struct timeval *)&timeout);
    if (counter <= 0) {
      system_error("In main() -> select() failed");
      /*error*/ exit(EXIT_FAILURE);
    }

    temp = host->node_list;
    for (/* X */; counter > 0; counter--) {
      memset(&buffer, 0, SIZE);
      if (FD_ISSET(STDIN_FILENO, &working_set)) { // KEYBOARD
        if (read(STDIN_FILENO, buffer, SIZE) == -1) {
          system_error("In main() -> read() failed");
          /*error*/ exit(EXIT_FAILURE);
        }
        /* Process standard input */
        process_stdin_input(buffer, host);
        FD_CLR(STDIN_FILENO, &working_set);
        continue;
      }

      else if (FD_ISSET(host->listen_fd, &working_set)) { // SOCKET DE LISTEN
        if ((new_fd = accept(host->listen_fd, &in_addr, &in_addrlen)) == -1) {
          system_error("In main() -> accept() failed");
          /*error*/ exit(EXIT_FAILURE);
        }
        if (read(new_fd, buffer, SIZE) == -1) {
          system_error("In main() -> read() failed");
          /*error*/ exit(EXIT_FAILURE);
        }
        printf("msg received: %s\n", buffer);
        /* Process the new accepted file descriptor */
        process_newfd(host, new_fd, buffer);
        FD_CLR(host->listen_fd, &working_set);
        continue;
      }

      else { // SOCKETS DOS NÓS VIZINHOS
        printf("teste\n");
        for (; temp != NULL && !FD_ISSET(temp->fd, &working_set); temp = temp->next)
          ;
        /*! TODO: Função que processa o fildes do nó vizinho pronto */
        FD_CLR(temp->fd, &working_set);
      }
    }
  }
}
