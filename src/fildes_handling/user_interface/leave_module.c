#include "leave_module.h"
#include "../../common/prompts.h"
#include "../../common/utils.h"
#include "../../error_handling/error_messages.h"
#include "../../protocols/UDP.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 16

/**
 * @brief Clears the host structure and frees memory allocated for it.
 *
 * @param host: pointer to the `host` structure to be cleared.
 */
void clear_host(host *host) {
  // Free memory for network and ID
  free(host->net);
  free(host->ID);
  host->net = host->ID = NULL;

  // Close external connection if it exists
  if (host->ext != NULL && host->ext->fd != -1) {
    close(host->ext->fd);
    host->ext = NULL;
  }

  // Delete nodes list
  node *current_node = host->node_list;
  while (current_node != NULL) {
    node *next_node = current_node->next;
    free_node(current_node);
    current_node = next_node;
  }
  host->node_list = NULL;
  free_node(host->bck);
  host->bck = NULL;

  // Delete names list
  names *current_name = host->names_list;
  while (current_name != NULL) {
    names *next_name = current_name->next;
    free(current_name);
    current_name = next_name;
  }
  host->names_list = NULL;

  // Reset tab_expedicao
  memset(host->tab_expedicao, -1, sizeof(host->tab_expedicao));
}

/**
 * @brief Leaves the current network and unregisters the host from it.
 * @note If the flag is DJOIN, no UNREG message will be sent.
 * @note Sends UNREG message to the node server, withdraws the host from all nodes
 * and clears the host structure.
 *
 * @param host: pointer to the `host` structure.
 * @param flag: used to specify the type of leaving. If flag is DJOIN, clears the
 * host and returns.
 */
void leave_network(host *host, int flag) {
  // Check if the host is registered in a network
  if (!host->net) {
    printf(YELLOW "[NOTICE]" RESET " Host not registered in a network\n");
    return;
  }

  // If flag is DJOIN, clear the host and return
  if (flag == DJOIN) {
    /*! TODO: send withdraw*/
    clear_host(host);
    return;
  }

  // Send UNREG message to the node server
  char msg_to_send[SIZE];
  snprintf(msg_to_send, SIZE, "UNREG %s %s\n", host->net, host->ID);
  char *msg_received = send_message_UDP(host->uip, msg_to_send);

  // Check if message was successfully sent
  if (!msg_received) {
    fprintf(stderr,
            YELLOW "[NOTICE]" RESET " Failed to unregister from the network %s\n",
            host->net);
    UDP_server_message(1, msg_received);
    return;
  }

  // Check if the received message is "OKUNREG"
  if (strcmp(msg_received, "OKUNREG") != 0) {
    fprintf(stderr,
            YELLOW "[NOTICE]" RESET " Failed to unregister from the network %s\n",
            host->net);
    UDP_server_message(1, msg_received);
    free(msg_received);
    return;
  }

  // Print success message and notify UDP server
  printf(YELLOW "[NOTICE]" RESET " Successfully unregistered from the network %s\n",
         host->net);
  UDP_server_message(0, msg_received);
  free(msg_received);

  // Clear host structure
  clear_host(host);
}

/**
 * @brief This function is called when the program needs to exit. It leaves the
 * network by calling `leave_network()` with the provided flag parameter.
 *
 * - If leaving the network fails, it clears the host structure by calling
 * `clear_host()`.
 * - After that, it closes the listening socket file descriptor using `close()`.
 * - Finally, it frees the memory allocated for the user interface and host structure
 * using `free()`. It also prints a message to indicate that the user interface is
 * turned off.
 *
 * @param host: a pointer to a `host` structure that represents the current host in
 * the network
 * @param flag: an integer value that indicates which join command connected the host
 * to the network
 */
int exit_program(host *host, int flag) {
  // Leave the network before exiting the program
  leave_network(host, flag);

  // If leaving the network failed, clear the host structure
  if (host->ext != NULL) {
    clear_host(host);
  }

  close(host->listen_fd);
  // free(host->uip->IP), free(host->uip->regIP);
  free(host->uip);
  free(host);

  user_interface_toggle(OFF);
  return 0;
}
