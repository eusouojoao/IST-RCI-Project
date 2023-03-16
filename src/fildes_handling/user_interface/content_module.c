#include "content_module.h"
#include "../../common/struct.h"
#include "../../common/utils.h"
#include "../../error_handling/error_checking.h"
#include "../socket_protocols_interface/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Creates a new node for a linked list of names.
 *
 * This function dynamically allocates memory for a new node in a linked list of
 * names, initializes its fields with the given name and next node, and returns a
 * pointer to the new node. If memory allocation fails, ...
 *
 * @param name A pointer to a string containing the name to store in the new node.
 * @param next A pointer to the next node in the linked list.
 *
 * @return A pointer to the newly created node.
 */
names *new_names(char *name, names *next) {
  names *new_name = (names *)malloc(sizeof(struct names));
  if (new_name == NULL) {
    perror("Error: malloc failed");
    return NULL;
  }

  new_name->name = name;
  new_name->next = next;

  // Return a pointer to the new node
  return new_name;
}

/**
 * @brief Insert a name in the names list of the host.
 * @param name The name to be inserted.
 * @param host The host where the name should be inserted.
 * @return 1 on success, -1 if the name is too long, 0 if the name already
 * exists in the list.
 */
int insert_name(host *host, char *buffer) {
  if (number_of_command_arguments(buffer, ' ') > 1) {
    return -1;
  }

  char *name = strdup(buffer + strlen("create "));
  if (name == NULL) {
    perror("Error: malloc failed");
    return -1;
  }
  name[strlen(name) - 1] = '\0';

  if (check_name(name) == -1) {
    free(name);
    return -1; // Failure, name is too long or contains non-alphanumeric entries
  }

  // Check if name already exists in the list
  for (names *current = host->names_list; current != NULL; current = current->next) {
    if (strcmp(current->name, name) == 0) {
      free(name);
      return 0; // Failure, name already exists in the list
    }
  }

  // Insert new name at the beginning of the list
  host->names_list = new_names(name, host->names_list);
  return 1; // Success, name was inserted
}

/**
 * @brief Delete a name from the names list of the host.
 *
 * @param name_to_delete The name to be deleted.
 * @param host The host where the name should be deleted from.
 * @return Returns 1 on success, -1 if the name is too long, 0 if the name does not
 * exist in the list.
 */
int delete_name(host *host, char *buffer) {
  if (number_of_command_arguments(buffer, ' ') > 1) {
    return -1;
  }

  char *name_to_delete = strdup(buffer + strlen("delete "));
  if (name_to_delete == NULL) {
    perror("Error: malloc failed");
    return -1;
  }
  name_to_delete[strlen(name_to_delete) - 1] = '\0';

  if (check_name(name_to_delete) == -1) {
    return -1; // Name too long, or contains non-alphanumeric entries
  }

  names **p = &host->names_list;
  while (*p != NULL) {
    if (strcmp((*p)->name, name_to_delete) == 0) {
      names *temp = *p;
      *p = (*p)->next;
      printf("temp->name: %s", temp->name);
      free(temp->name);
      free(temp);
      return 1; // Success, name deleted
    }
    p = &(*p)->next;
  }

  return 0; // Failure, name not found
}

void get_name(host *host, char *buffer) {
  char dest[32] = {'\0'}, name[100] = {'\0'};
  char protocol_msg[256] = {'\0'};

  // Parse the command and store the destination and name.
  if (sscanf(buffer, "get %s %s", dest, name) < 2) {
    // Add proper error handling
    printf("Less than 2 arguments\n");
    return;
  }

  // Check if the name is valid
  if (check_name(name) == -1) {
    // Add proper error handling
    printf("Invalid name\n");
    return;
  }

  // If the destination is the current host, search for the name in the names list.
  if (strcmp(host->ID, dest) == 0) {
    if (find_name(name, host)) {
      printf("Name: `%s` found on node %s\n", name, dest);
    } else {
      printf("Name: `%s` not found on node %s\n", name, dest);
    }
    return;
  }

  // Prepare the QUERY protocol message
  snprintf(protocol_msg, 256, "QUERY %s %s %s\n", dest, host->ID, name);

  // Check if the destination node is a known neighbor
  node *neighbour = check_route(host, dest);
  if (neighbour != NULL) {
    // Send the QUERY message to the known neighbor
    if (write(neighbour->fd, protocol_msg, 256) == -1) {
      printf("Error sending QUERY to known neighbor\n");
    }
    return;
  }

  // If the destination node is not in the routing table, broadcast the QUERY message
  // to all neighbors
  send_protocol_messages(host, -1, protocol_msg);
}
