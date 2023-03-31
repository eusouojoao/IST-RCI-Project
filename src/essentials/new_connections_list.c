#include "new_connections_list.h"
#include "../error_handling/error_messages.h"
#include "circular_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DELTA_T_MULTIPLIER 3000

/**
 * @brief Create a new connection struct.
 * @param new_fd: the file descriptor for the new connection
 * @return A pointer to the newly created new_connection struct.
 */
static new_connection *create_new_connection(int new_fd) {
  new_connection *connection = (new_connection *)malloc(sizeof(new_connection));
  if (connection == NULL) {
    system_error("malloc() failed");
    return NULL;
  }

  connection->cb = (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
  if (connection->cb == NULL) {
    system_error("malloc() failed");
    free(connection);
    return NULL;
  }

  cb_init(connection->cb);
  connection->new_fd = new_fd;
  connection->next = NULL;
  connection->time_to_live = DELTA_T_MULTIPLIER;

  return connection;
}

/**
 * @brief Insert a new connection into the new_connections_list of the host.
 * @param host: pointer to the host struct
 * @param new_fd: the file descriptor for the new connection
 * @param buffer: the data to be stored in the connection's circular buffer
 */
void insert_new_connection(host *host, int new_fd, char *buffer) {
  new_connection *connection = create_new_connection(new_fd);
  if (connection == NULL) {
    die_with_system_error(host, NULL);
  }

  size_t len = strlen(buffer);
  if (cb_write(connection->cb, buffer, len) != len) {
    close(connection->new_fd);
    free(connection->cb), free(connection);
    return;
  }

  connection->next = host->new_connections_list;
  host->new_connections_list = connection;
}

/**
 * @brief Remove a connection from the new_connections_list by file descriptor.
 * @param host: pointer to the host struct
 * @param new_fd: the file descriptor of the connection to remove
 * @return 1 if the connection was removed, 0 if the connection was not found.
 */
int remove_new_connection(host *host, int new_fd) {
  new_connection *temp = host->new_connections_list;
  new_connection *prev = NULL;

  if (temp != NULL && temp->new_fd == new_fd) {
    host->new_connections_list = temp->next;
    free(temp->cb), free(temp);
    return 1;
  }

  while (temp != NULL && temp->new_fd != new_fd) {
    prev = temp;
    temp = temp->next;
  }

  if (temp == NULL) {
    return 0;
  }

  prev->next = temp->next;
  free(temp->cb), free(temp);

  return 1;
}

/**
 * @brief Delete the entire new_connections_list and free the memory.
 * @param host: pointer to the host struct
 */
void delete_new_connections_list(host *host) {
  new_connection *temp = NULL;

  while (host->new_connections_list != NULL) {
    temp = host->new_connections_list;
    host->new_connections_list = temp->next;
    close(temp->new_fd);
    free(temp->cb), free(temp);
  }

  host->new_connections_list = NULL;
}
