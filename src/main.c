#include "arguments_parser.h"
#include "essentials/host_handling.h"
#include "fildes_handling/core/TCP.h"
#include "fildes_handling/descriptor_control/descriptor_control.h"
#include "misc/handle_terminal.h"
#include "misc/prompts.h"

#include <signal.h>
#include <stdlib.h>
#include <time.h>

void signal_setup(void) {
  // Set up the custom signal handler and enable SA_RESTART
  struct sigaction action;
  action.sa_handler = handle_sigquit;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  // Register the signal handler
  if (sigaction(SIGQUIT, &action, NULL) == -1) {
    exit(1);
  }

  // Ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char *argv[]) {
  // Seed the rand function
  srand((unsigned int)time(NULL));

  // Setup signal handling
  signal_setup();

  // Map SIGQUIT to CTRL+L to clear the terminal
  struct termios original_termios;
  modify_termios(&original_termios);

  // Parse and setup user arguments
  user_args *uip = NULL;
  uip = arguments_parser(argc, argv);

  // Initialize the host structure with the user supplied arguments
  host *host = init_host(uip);
  host->listen_fd = create_listen_socket(uip);

  // select() working variables
  fd_set working_set; // Read file descriptors set
  int counter = 0;    // Number of descriptors that became ready

  // User interface engage
  print_header();
  user_interface_toggle(ON);

  // Working buffer
  char buffer[256] = {'\0'};

  int r = 0; // Stores the return values
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
