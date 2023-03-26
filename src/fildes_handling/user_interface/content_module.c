#include "content_module.h"
#include "../../common/struct.h"
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
    printf("enro\n");
    return -1;
  }

  char *name = strdup(buffer + strlen("create "));
  if (name == NULL) {
    perror("Error: malloc failed");
    return -1;
  }
  name[strlen(name)] = '\0';

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

  char name_to_delete[128] = {'\0'};
  if (sscanf(buffer, "delete %127s\n", name_to_delete) != 1) {
    return -1;
  }

  if (check_name(name_to_delete) == -1) {
    return -1; // Name too long, or contains non-alphanumeric entries
  }

  names **p = &host->names_list;
  while (*p != NULL) {
    if (strcmp((*p)->name, name_to_delete) == 0) {
      names *temp = *p;
      *p = (*p)->next;
      free(temp->name);
      free(temp);
      return 1; // Success, name deleted
    }
    p = &(*p)->next;
  }

  return 0; // Failure, name not found
}

/**
 * @brief Parses the input buffer and extracts destination and name.
 *
 * @param buffer Input buffer.
 * @param dest Destination buffer.
 * @param name Name buffer.
 * @return Returns 1 if successfully parsed, 0 otherwise.
 */
int parse_get_name_command(char *buffer, char *dest, char *name) {
  if (sscanf(buffer, "get %s %s\n", dest, name) != 2) {
    printf("Less than 2 arguments\n");
    return 0;
  }
  return 1;
}

/**
 * @brief Handles the case where the destination node is the current host.
 *
 * @param host: Pointer to the host structure.
 * @param dest: destination
 * @param name: name buffer
 * @return 1 if the destination is the current host, 0 otherwise.
 */
int handle_destination_is_current_host(host *host, char *dest, char *name) {
  if (strcmp(host->ID, dest) == 0) {
    if (find_name(name, host)) {
      printf("Name: `%s` found on node %s\n", name, dest);
    } else {
      printf("Name: `%s` not found on node %s\n", name, dest);
    }
    return 1;
  }
  return 0;
}

void get_name(host *host, char *buffer) {
  char dest[32] = {'\0'}, name[100] = {'\0'};

  if (!parse_get_name_command(buffer, dest, name)) {
    return;
  }

  if (check_name(name) == -1) {
    printf("Invalid name\n");
    return;
  }

  if (handle_destination_is_current_host(host, dest, name)) {
    return;
  }

  // Generate the protocol message
  char protocol_msg[256] = {'\0'};
  snprintf(protocol_msg, 256, "QUERY %s %s %s\n", dest, host->ID, name);

  send_message_to_neighbours(host, -1, dest, protocol_msg);
}
