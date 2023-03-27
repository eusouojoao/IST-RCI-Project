#include "common/arguments_parser.h"
#include "common/handle_terminal.h"
#include "common/prompts.h"
#include "common/utils.h"
#include "fildes_handling/core/TCP.h"
#include "fildes_handling/descriptor_control.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdio.h>

void signal_setup(void) {
  // Set up the custom signal handler and enable SA_RESTART
  struct sigaction action;
  action.sa_handler = handle_sigquit;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  // Register the signal handler
  if (sigaction(SIGQUIT, &action, NULL) == -1) {
    exit(EXIT_FAILURE);
  }

  // Ignore SIGPIPE, SIGINT and SIGTSTP
  signal(SIGPIPE, SIG_IGN);
  // signal(SIGINT, SIG_IGN);  // CTRL + C
  signal(SIGTSTP, SIG_IGN); // CTRL + Z
}

int main(int argc, char *argv[]) {
  // Seed the rand function
  srand((unsigned int)time(NULL));

  // Parse and setup user arguments
  user_args *uip = NULL;
  uip = arguments_parser(argc, argv);

  // Initialize the host structure with the user supplied arguments
  host *host = init_host(uip);
  host->listen_fd = create_listen_socket(uip);

  // select() working variables
  fd_set working_set; // Read file descriptors set
  int counter = 0;    // Number of descriptors that became ready

  // Setup signal handling
  signal_setup();

  // Map SIGQUIT to CTRL+L to clear the terminal
  struct termios original_termios;
  modify_termios(&original_termios);

  // User interface engage
  print_header();
  user_interface_toggle(ON);

  int r = 0; // Stores the return values
  char buffer[256] = {'\0'};

  while (ON) {
    // Print prompt
    prompt();

    // Update the file descriptor's working set
    update_working_set(host, &working_set);

    // Wait for input
    if ((r = wait_for_ready_fildes(host, &working_set, &counter, NULL)) == -1) {
      // Exiting... fatal error
      break;
    }

    // Process input
    if ((r = fildes_control(host, &working_set, &counter, buffer)) == 0) {
      // Exiting...
      break;
    }
  }

  restore_termios(&original_termios);
  return r;
}
