#include "show_module.h"
#include "../../error_handling/error_messages.h"
#include "../../misc/prompts.h"

#include <stdio.h>
#include <string.h>

#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])

/**
 * @brief Wrapper function to display network information.
 *
 * This function takes a command and a buffer containing a "show" command, and
 * displays the corresponding network information based on the command. The supported
 * commands are: SHOW_TOPOLOGY, SHOW_NAMES, and SHOW_ROUTING. If the command is not
 * imediatelly recognized, the function tries to parse the "show" subcommand from the
 * buffer.
 *
 * @param host: pointer to the host structure.
 * @param cmd: command to execute (SHOW_TOPOLOGY, SHOW_NAMES, or SHOW_ROUTING).
 * @param buffer: null-terminated string containing the "show" command and its
 * arguments.
 */
void show_wrapper(host *host, user_command cmd, char *buffer) {
  if (cmd == SHOW) { // If cmd is not provided, parse the command from the buffer

    // Attempt to parse a generic "show" command from the buffer
    char subcmd[128] = {'\0'};
    if (sscanf(buffer, "show %s\n", subcmd) == 1) {
      if (strcmp(subcmd, "topology") == 0) {
        cmd = SHOW_TOPOLOGY;
      } else if (strcmp(subcmd, "names") == 0) {
        cmd = SHOW_NAMES;
      } else if (strcmp(subcmd, "routing") == 0) {
        cmd = SHOW_ROUTING;
      } else {
        user_input_error("Command not found", buffer,
                         "The available `show` commands are: show names (sn); show "
                         "topology (st); show routing (sr)");
        return;
      }
    } else {
      user_input_error("Invalid command format", buffer,
                       "The `show` commands must have 2 words separated by a space. E.g. "
                       "show names (sn)");
      return;
    }
  }

  switch (cmd) {
  case SHOW_TOPOLOGY:
    show_topology(host);
    break;
  case SHOW_NAMES:
    show_names(host);
    break;
  case SHOW_ROUTING:
    show_routes(host);
    break;
  default:
    break;
  }
}

/**
 * @brief Prints the topology of a host.
 *
 * This function prints the ID and network of the host, as well as the IDs of its
 * internal, external, and backup neighbors.
 *
 * @param host: pointer to the host struct.
 */
void show_topology(host *host) {
  if (host->net == NULL) {
    user_error("Host must be registered in a network to have a topology.");
    return;
  }

  printf("╔═══════════════════════════════════════╗\n");
  printf("║          Host Topology: ID %s         ║\n", host->ID);
  printf("║              Network %s              ║\n", host->net);
  printf("╠═══════════════════════════════════════╣\n");
  printf("║  Backup Node:                         ║\n");
  if (host->bck == NULL) {
    printf("║  %s\t%s\t%d\n", host->ID, host->uip->IP, host->uip->TCP);
  } else {
    printf("║  %s\t%s\t%d\n", host->bck->ID, host->bck->IP, host->bck->TCP);
  }

  if (host->ext != NULL) { // If not the only node in the network

    printf("╠═══════════════════════════════════════╣\n");
    printf("║  External Neighbor:                   ║\n");
    printf("║  %s\t%s\t%d\n", host->ext->ID, host->ext->IP, host->ext->TCP);
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  Internal Neighbors List:             ║\n");

    node *node_ptr = host->node_list;
    while (node_ptr != NULL) {
      if (strcmp(node_ptr->ID, host->ext->ID) != 0) {
        // Don't print external node as internal
        printf("║  %s\t%s\t%d\n", node_ptr->ID, node_ptr->IP, node_ptr->TCP);
      }
      node_ptr = node_ptr->next;
    }
  } else {
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  External Neighbor:                   ║\n");
    printf("║                (None)                 ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  Internal Neighbors List:             ║\n");
    printf("║                (None)                 ║\n");
  }

  printf("╚═══════════════════════════════════════╝\n");
}

/**
 * @brief Prints the names of a host.
 * @param host: pointer to the host struct.
 */
void show_names(host *host) {
  const names *names_ptr = host->names_list;

  printf("╔══════════════════════════════════════════════╗\n");
  printf("║  Names List:                                 ║\n");
  printf("╠══════════════════════════════════════════════╣\n");

  if (names_ptr == NULL) {
    printf("║                   (None)                     ║\n");
  }

  while (names_ptr != NULL) {
    printf("║ -> %s\n", names_ptr->name);
    names_ptr = names_ptr->next;
  }

  printf("╚══════════════════════════════════════════════╝\n");
}

/**
 * @brief Prints the routing table the current host.
 * @param host: pointer to the host struct.
 */
void show_routes(host *host) {
  if (host->net == NULL) {
    user_error("Host must be registered in a network to have a forwarding table.");
    return;
  }

  printf("╔══════════════════════════════════════════════╗\n");
  printf("║            Routing Table: Host %s            ║\n", host->ID);
  printf("║                Network %s                   ║\n", host->net);
  printf("╠═════════════╦════════════════════════════════╣\n");
  printf("║ Destination ║            Neighbor            ║\n");
  printf("╠═════════════╬════════════════════════════════╣\n");
  for (size_t i = 0; i < ELEMENTS; i++) {
    if (host->tab_expedicao[i] != -1) {
      printf("║     %02zu      ║               %02d               ║\n", i,
             host->tab_expedicao[i]);
    }
  }
  printf("╚═════════════╩════════════════════════════════╝\n");
}
