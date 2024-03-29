#include "query_module.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../../misc/prompts.h"
#include "../core/TCP.h"
#include "common.h"
#include "protocol_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief  Reads and saves query message from user
 * @param  buffer: buffer with the message
 * @param  dest: destination node
 * @param  orig: origin node
 * @param  name: name of the content that is being searched
 * @retval 1 OK
 *         0 Error
 */
int parse_query_message(char *buffer, char *dest, char *orig, char *name) {
  // Parse the command and store the destination and name.
  if (sscanf(buffer, "QUERY %s %s %s\n", dest, orig, name) != 3) {
    return 0;
  }
  return 1;
}

/**
 * @brief Processes a QUERY message, forwards it if needed, and generates a
 * response.
 *
 * @param host: pointer to the host structure.
 * @param sender: pointer to the sender node structure.
 * @param buffer: pointer to the buffer containing the QUERY message.
 */
void process_query(host *host, node *sender, char *buffer) {
  char dest[32] = {'\0'}, orig[32] = {'\0'}, name[100] = {'\0'};

  if (!parse_query_message(buffer, dest, orig, name)) {
    return;
  }

  if (!check_id(dest) || !check_id(orig)) {
    return;
  }

  // Check if the name is valid
  if (check_name(name) == -1) {
    return;
  }

  // Update forwarding table with the sender node
  insert_in_forwarding_table(host, atoi(sender->ID), atoi(sender->ID));
  insert_in_forwarding_table(host, atoi(orig), atoi(sender->ID));

  // Initialize the protocol message buffer
  char protocol_msg[256] = {'\0'};

  // If the destination is the current host, search for the name in the names list.
  if (strcmp(host->ID, dest) == 0) {
    if (find_name(name, host)) {
      snprintf(protocol_msg, 256, "CONTENT %s %s %s\n", orig, dest, name);
    } else {
      snprintf(protocol_msg, 256, "NOCONTENT %s %s %s\n", orig, dest, name);
    }
    // Send the CONTENT or NOCONTENT message to the sender
    if (write_msg_TCP(sender->fd, protocol_msg, strlen(protocol_msg)) == -1) {
      return;
    }

    return;
  }

  send_message_to_neighbours(host, sender->fd, dest, buffer);
}

/**
 * @brief  Reads and saves the arguments of a CONTENT/NOCONTENT protocol message
 * @param  buffer: buffer with the message
 * @param  dest: destination node
 * @param  orig: origin node
 * @param  name: name of the content that was searched
 * @retval 1 OK
 *         0 Error
 */
int parse_content_message(char *buffer, char *orig, char *dest, char *name,
                          protocol_command cmd) {
  if (cmd == CONTENT) {
    if (sscanf(buffer, "CONTENT %s %s %s\n", dest, orig, name) != 3) {
      return 0;
    }
  } else if (cmd == NOCONTENT) {
    if (sscanf(buffer, "NOCONTENT %s %s %s\n", dest, orig, name) != 3) {
      return 0;
    }
  }

  return 1;
}

/**
 * @brief Handles the response (CONTENT or NOCONTENT) for a QUERY request.
 *
 * This function processes the response to a QUERY request, forwarding it to the
 * appropriate neighboring node(s) or displaying the response on the current
 * host. It is called when a CONTENT or NOCONTENT message is received by the
 * host.
 *
 * @param host: pointer to the current host structure
 * @param sender: pointer to the sender node structure
 * @param buffer: buffer containing the received message
 * @param cmd: enum value representing the protocol command (CONTENT or
 * NOCONTENT)
 */
void handle_content_response(host *host, node *sender, char *buffer, protocol_command cmd) {
  char dest[32] = {'\0'}, orig[32] = {'\0'}, name[100] = {'\0'};

  if (!parse_content_message(buffer, orig, dest, name, cmd)) {
    return;
  }

  if (!check_id(dest) || !check_id(orig)) {
    return;
  }

  if (check_name(name) == -1) {
    return;
  }

  // Update forwarding table with the sender node
  insert_in_forwarding_table(host, atoi(sender->ID), atoi(sender->ID));
  insert_in_forwarding_table(host, atoi(orig), atoi(sender->ID));

  // If the origin matches the current host ID, print the response
  if (strcmp(host->ID, dest) == 0) {
    clear_stdout_line();
    if (cmd == CONTENT) {
      printf(YELLOW "[CONTENT]" RESET " Name: `%s` " GREEN "found" RESET " on node %s\n", name,
             orig);
    } else {
      printf(YELLOW "[NOCONTENT]" RESET " Name: `%s` " RED "not found" RESET " on node %s\n",
             name, orig);
    }
    prompt();

    return;
  }

  send_message_to_neighbours(host, sender->fd, dest, buffer);
}
