#include "common/arguments_parser.h"
#include "common/utils.h"
#include "descriptor_utils/socket_descriptor_control.h"
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
  uip = arguments_parser(argc, argv);

  /* Inicializar a estrutura do host e temp */
  host *host = init_host(uip);
  node *temp = NULL;

  /* File descriptors e counter do select() */
  fd_set working_set; // set with the read file descriptors
  int new_fd = -1, counter = 0;
  host->listen_fd = create_listen_socket(uip);

  /* Working buffer */
  char buffer[SIZE];
  memset(buffer, 0, SIZE);

  struct sockaddr in_addr;
  socklen_t in_addrlen = sizeof(in_addr);

  /* Inicializar a estrutura timeval para o timeout */
  struct timeval timeout = {.tv_sec = 600, .tv_usec = 0}; // 600s = 10min

  CLEAR_STDIN();
  printf(BLUE "%*s User interface [" GREEN "ON" BLUE "]\n" RESET, 6, "");

  for (/* for */; /* ever */; /* ! */) {
    printf(GREEN "<USER> " RESET);
    fflush(stdout);
    /* Update the file descriptor's working set */
    update_working_set(host, &working_set);
    int maxfd = get_maxfd(host);
    printf("maxfd = %d\n", maxfd);
    counter = select(maxfd + 1, &working_set, (fd_set *)NULL, (fd_set *)NULL,
                     (struct timeval *)&timeout);
    if (counter <= 0) {
      system_error("In main() -> select() failed");
      /*error*/ exit(EXIT_FAILURE);
    }

    printf("Counter = %d\n", counter);

    for (temp = host->node_list; counter > 0; memset(buffer, 0, SIZE), counter--) {
      if (FD_ISSET(STDIN_FILENO, &working_set)) { // KEYBOARD
        if (read(STDIN_FILENO, buffer, SIZE) == -1) {
          system_error("In main() -> read() failed");
          /*error*/ exit(EXIT_FAILURE);
        }
        /* Process standard input - keyboard */
        process_stdin_input(host, buffer);
        FD_CLR(STDIN_FILENO, &working_set);
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
        /* Process the new accepted file descriptor */
        process_new_fd(host, new_fd, buffer);
        FD_CLR(host->listen_fd, &working_set);
      }

      else { // SOCKETS DOS NÓS VIZINHOS
        for (; temp != NULL; temp = temp->next) {
          if (FD_ISSET(temp->fd, &working_set)) {
            if (read(temp->fd, buffer, SIZE) == -1) {
              system_error("In main() -> read() failed");
              /*error*/ exit(EXIT_FAILURE);
            }
            /* Process extern and intern node communication */
            process_neighbour_node_fd(host, temp, buffer);
            FD_CLR(temp->fd, &working_set);
          }
        }
      }
    }
  }
}
