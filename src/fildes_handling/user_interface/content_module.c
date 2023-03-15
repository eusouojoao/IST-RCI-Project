#include "content_module.h"
#include "../../error_handling/error_checking.h"

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
 * @return Returns 1 on success, -1 if the name is too long, 0 if the name already
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
