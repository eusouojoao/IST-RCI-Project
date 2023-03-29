#include "update_backup_module.h"
#include "../../error_handling/error_checking.h"
#include "../../essentials/host_handling.h"
#include "delete_node_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 128

/**
 * @brief A wrapper function for handling a flag with set and get operations.
 * @param value: the value to set the flag to if the set operation is requested
 * @param set: if non-zero, sets the flag to the provided value; otherwise, retrieves the
 * current flag value
 * @return the current value of the flag.
 */
int flag_wrapper(int value, int set) {
  // Declare a static flag variable that maintains its value between function calls
  static int flag = 0;

  // If the 'set' parameter is non-zero, set the flag to the provided value
  if (set) {
    flag = value;
  }

  // Return the current value of the flag
  return flag;
}

/**
 * @brief Sets the flag to a specified value.
 * @param value: The value to set the flag to.
 */
void set_flag(int value) {
  // Call the flag_wrapper() function with 'set' parameter as 1 to set the flag to the
  // specified value
  flag_wrapper(value, 1);
}

/**
 * @brief Retrieves the current value of the flag.
 * @return The current value of the flag.
 */
int get_flag() {
  // Call the flag_wrapper() function with 'set' parameter as 0 to retrieve the current value
  // of the flag
  return flag_wrapper(0, 0);
}

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
}
