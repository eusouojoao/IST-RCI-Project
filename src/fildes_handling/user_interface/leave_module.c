#include "leave_module.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../../misc/prompts.h"
#include "../core/UDP.h"
#include "user_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 16

/**
 * @brief Deletes the names list in the host structure and frees memory.
 *
 * @param host: pointer to the host structure containing the names list to be deleted
 */
void delete_names_list(host *host) {
  names *current_name = host->names_list;
  names *next_name;

  while (current_name != NULL) {
    next_name = current_name->next;
    free(current_name->name);
    free(current_name);
    current_name = next_name;
  }

  host->names_list = NULL;
}

/**
 * @brief Deletes the nodes list in the host structure and frees memory.
 *
 * @param host: pointer to the host structure containing the nodes list to be deleted
 */
void delete_nodes_list(host *host) {
  node *current_node = host->node_list;
  node *next_node;

  while (current_node != NULL) {
    next_node = current_node->next;
    free_node(current_node);
    current_node = next_node;
  }

  host->node_list = NULL;
  free_node(host->bck);
  host->bck = NULL;
}

/**
 * @brief  Resets forwarding table (clears all known paths)
 * @note
 * @param  *host: Pointer to the host structure
 * @retval None
 */
void reset_forwarding_table(host *host) {
  memset(host->tab_expedicao, -1, sizeof(host->tab_expedicao));
}

/**
 * @brief Clears the host structure and frees memory allocated for it.
 *
 * @param host: Pointer to the host structure to be cleared
 */
void clear_host(host *host) {
  // Free memory for network and ID
  free(host->net);
  free(host->ID);
  host->net = NULL;
  host->ID = NULL;

  // Close external connection if it exists
  if (host->ext != NULL && host->ext->fd != -1) {
    close(host->ext->fd);
    host->ext = NULL;
  }

  // Delete nodes list
  delete_nodes_list(host);

  // Resets the forwarding table
  reset_forwarding_table(host);
}

/**
 * @brief Makes the host leave the network and unregister it.
 *
 * @param host: pointer to the host structure to leave the network and unregister
 * @param flag: indicating whether to only clear the host (DJOIN) or to also send an UNREG
 * message
 */
void leave_network(host *host, user_command flag) {
  // Check if the host is registered in a network
  if (!host->net) {
    printf(YELLOW "[NOTICE]" RESET " Host not registered in a network\n");
    return;
  }

  // If flag is DJOIN, clear the host and return
  if (flag == DJOIN) {
    clear_host(host);
    return;
  }

  // Send UNREG message to the node server
  char msg_to_send[SIZE];
  snprintf(msg_to_send, SIZE, "UNREG %s %s\n", host->net, host->ID);
  char *msg_received = send_and_receive_msg_UDP(host->uip, msg_to_send);

  // Check if message was successfully sent
  if (!msg_received) {
    fprintf(stderr, YELLOW "[NOTICE]" RESET " Failed to unregister from the network %s\n",
            host->net);
    return;
  }

  // Check if the received message is "OKUNREG"
  if (strcmp(msg_received, "OKUNREG") != 0) {
    fprintf(stderr, YELLOW "[NOTICE]" RESET " Failed to unregister from the network %s\n",
            host->net);
    free(msg_received);
    return;
  }

  // Print success message and notify UDP server
  printf(YELLOW "[NOTICE]" RESET " Successfully unregistered from the network %s\n",
         host->net);
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
 * @param host: a pointer to a host structure that represents the current host in
 * the network
 * @param flag: indicates which join command connected the host to the network
 */
int exit_program(host *host, user_command flag) {
  // Leave the network before exiting the program
  leave_network(host, flag);

  // If leaving the network failed, clear the host structure
  if (host->ext != NULL) {
    clear_host(host);
  }

  // Delete names list
  delete_names_list(host);

  close(host->listen_fd);
  free(host->uip);
  free(host);

  user_interface_toggle(OFF);
  return 0;
}
