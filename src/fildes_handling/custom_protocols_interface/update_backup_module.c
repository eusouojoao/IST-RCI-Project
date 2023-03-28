#include "update_backup_module.h"
#include "../../error_handling/error_checking.h"
#include "../../essentials/host_handling.h"
#include "delete_node_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 128

/**
 * @brief Update the backup node information in the host structure.
 * @param host: pointer to the host structure.
 * @param buffer: buffer containing the new backup node information.
 */
void update_backup(host *host, char *buffer) {
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  free_node(host->bck), host->bck = NULL;

  if (sscanf(buffer, "EXTERN %s %s %s\n", node_ID, node_IP, node_TCP) != 3) {
    delete_node(host, host->ext->fd);
    return;
  }

  if (!check_node_parameters(node_ID, node_IP, node_TCP)) {
    delete_node(host, host->ext->fd);
    return;
  }

  if (strcmp(host->ID, node_ID) != 0) {
    host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
    insert_in_forwarding_table(host, atoi(node_ID), atoi(node_ID));
  }

  return;
}
