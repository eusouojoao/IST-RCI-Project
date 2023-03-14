#include "show_module.h"
#include "user_commands.h"

#include <stdio.h>
#include <string.h>

#define ELEMENTS sizeof(host->tab_expedicao) / sizeof(host->tab_expedicao[0])

void show_wrapper(host *host, int opt) {
  if (opt == SHOW_TOPOLOGY) {
    show_topology(host);
  } else if (opt == SHOW_NAMES) {
    show_names(host);
  } else if (opt == SHOW_ROUTING) {
    show_routes(host);
  }
  /*! TODO: Implementar caso geral do show por extenso */
  return;
}

/**
 * @brief Prints the topology of a host.
 *
 * This function prints the ID and network of the host, as well as the IDs of its
 * internal, external, and backup neighbors.
 *
 * @param host Pointer to the host struct.
 */
void show_topology(host *host) {
  node *node_ptr = host->node_list;

  printf("host->ext: %p, host->bck: %p\n", (void *)host->ext, (void *)host->bck);

  fprintf(stdout, "Topologia do host com: ID %s; Rede %s\n", host->ID, host->net);
  if (host->ext != NULL) { // se n~ao for o nó único na rede
    fprintf(stdout, "Nó de backup %s; Vizinho Externo %s\n",
            host->bck == NULL ? host->ID : host->bck->ID, host->ext->ID);
    fprintf(stdout, "Lista de Vizinhos Internos:");
    while (node_ptr != NULL) {
      if (strcmp(node_ptr->ID, host->ext->ID) != 0) {
        // Don't print external node as internal
        fprintf(stdout, " %s;", node_ptr->ID);
      }
      node_ptr = node_ptr->next;
    }
  }

  fprintf(stdout, "\n------------------------\n");
}

/**
 * @brief Prints the names of a host.
 *
 * @param host Pointer to the host struct.
 */
void show_names(host *host) {
  const names *names_ptr = host->names_list;
  printf("Nomes do host com: ID %s; Rede %s\n", host->ID, host->net);
  printf("Lista de Nomes:\n");
  while (names_ptr != NULL) {
    printf("%s\n", names_ptr->name);
    names_ptr = names_ptr->next;
  }
  printf("------------------------\n");
}

/**
 * @brief Prints the routing table of a host.
 *
 * @param host Pointer to the host struct.
 */
void show_routes(host *host) {
  fprintf(stdout, "Routing table of host %s (net %s):\n", host->ID, host->net);
  fprintf(stdout, "Destination  |  Next Hop\n");
  fprintf(stdout, "-------------|-------------\n");
  for (size_t i = 0; i < ELEMENTS; i++) {
    if (host->tab_expedicao[i] != -1) {
      fprintf(stdout, "%-13zu|  %-13d\n", i, host->tab_expedicao[i]);
    }
  }
  fprintf(stdout, "-------------|-------------\n");
}
