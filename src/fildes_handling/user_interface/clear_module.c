#include "clear_module.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../../misc/prompts.h"
#include "../user_interface/leave_module.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Clears the terminal window.
 */
void clear_window() {
  CLEAR_STREAM(STDIN_FILENO);
  user_interface_toggle(ON);
}

/**
 * @brief Clears the names list of the host.
 *
 * @param host: pointer to the host structure
 */
void clear_names(host *host) { delete_names_list(host); }

/**
 * @brief Resets the forwarding table of the host.
 *
 * @param host: pointer to the host structure
 */
void clear_routing(host *host) {
  if (host->net == NULL) {
    user_error("Host must be registered in a network to clear the forwarding table.");
    return;
  }

  reset_forwarding_table(host);
  insert_in_forwarding_table(host, atoi(host->ID), atoi(host->ID));
}

/**
 * @brief Wrapper function for handling 'clear' commands.
 *
 * This function handles different 'clear' commands, such as clear window, clear names, and
 * clear routing, based on the parsed command.
 *
 * @param host: pointer to the host structure
 * @param cmd: user command, indicating the clear operation to perform
 * @param buffer: the command buffer containing the user input
 */
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
        user_input_error("Command not found", buffer,
                         "The available `clear` commands are: clear names (cn); clear "
                         "window (cw); clear routing (cr)");
        return;
      }
    } else {
      user_input_error("Invalid command format", buffer,
                       "The `clear` commands must have 2 words separated by a space. E.g. "
                       "clear routing (cr)");
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
