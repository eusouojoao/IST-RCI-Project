#include "withdraw_module.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

void update_backup(host *host, char *buffer) {
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  sscanf(buffer, "EXTERN %s %s %s", node_ID, node_IP, node_TCP);
  if (check_node_parameters(node_ID, node_IP, node_TCP) == EXIT_FAILURE) {
    /* error */
    return;
  }

  free_node(host->bck);
  host->bck = create_new_node(node_ID, -1, node_IP, atoi(node_TCP));
  return;
}
