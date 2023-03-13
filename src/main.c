#include "common/arguments_parser.h"
#include "common/utils.h"
#include "error_handling/error_messages.h"
#include "fildes_handling/descriptor_control.h"
#include "protocols/TCP.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL)); // seed the rand function
  /* User arguments */
  user_args *uip = NULL;
  uip = arguments_parser(argc, argv);

  /* Inicializar a estrutura do host e temp */
  host *host = init_host(uip);

  /* File descriptors e counter do select() */
  fd_set working_set; // set with the read file descriptors
  host->listen_fd = create_listen_socket(uip);

  /* Inicializar a estrutura timeval para o timeout */
  struct timeval timeout = {.tv_sec = 600, .tv_usec = 0}; // 600s = 10min

  /* Ignore SIGPIPE */
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = SIG_IGN;
  if (sigaction(SIGPIPE, &act, NULL) == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }
  (void)act;

  /* User interface engage */
  CLEAR_STDIN();
  printf(BLUE "%*s User interface [" GREEN "ON" BLUE "]\n" RESET, 6, "");

  while (1) {
    int counter = 0; // will receive the number of descriptors that became ready
    /* Print prompt */
    printf(GREEN "<USER> " RESET);
    fflush(stdout);

    /* Update the file descriptor's working set */
    update_working_set(host, &working_set);

    /* Wait for input */
    if (wait_for_ready_fildes(host, &working_set, &counter, &timeout) == -1) {
      /*error*/ exit(EXIT_FAILURE);
    }

    /* Process input */
    if (fildes_control(host, &working_set, &counter) == -1) {
      /*error*/ exit(EXIT_FAILURE);
    }
  }
}
