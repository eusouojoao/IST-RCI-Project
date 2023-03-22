#include "clear_module.h"
#include "../../common/prompts.h"
#include "../user_interface/leave_module.h"
#include "user_commands.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void clear_window() { CLEAR_STREAM(STDIN_FILENO), user_interface_toggle(ON); }

void clear_names(host *host) { delete_names_list(host); }

void clear_routing(host *host) { reset_forwarding_table(host); }

void clear_wrapper(host *host, user_command cmd, char *buffer) {
  if (cmd == CLEAR) { // If the subcmd is not provided, parse the parse it from the buffer

    // Attempt to parse a generic "clear" command from the buffer
    char subcmd[128] = {'\0'};
    if (sscanf(buffer, "clear %s\n", subcmd) == 1) {
      if (strcmp(subcmd, "window") == 0) {
        cmd = CLEAR_WINDOW;
      } else if (strcmp(subcmd, "names") == 0) {
        cmd = CLEAR_NAMES;
      } else if (strcmp(subcmd, "routing") == 0) {
        cmd = CLEAR_ROUTING;
      } else {
        fprintf(stderr, "Command not found: %s", buffer);
        return;
      }
    } else {
      fprintf(stderr, "Invalid command format: %s", buffer);
      return;
    }
  }

  switch (cmd) {
  case CLEAR_WINDOW:
    clear_window();
    break;
  case CLEAR_NAMES:
    clear_names(host);
    break;
  case CLEAR_ROUTING:
    clear_routing(host);
    break;
  default:
    break;
  }
}
