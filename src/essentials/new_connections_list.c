#include "new_connections_list.h"
#include "../common/struct.h"
#include "../error_handling/error_messages.h"
#include "circular_buffer.h"

#include <stdlib.h>
#include <string.h>

new_connection *create_new_connection(int new_fd) {
  new_connection *connection = (new_connection *)malloc(sizeof(new_connection));
  if (connection == NULL) {
    system_error("malloc() failed");
    return NULL;
  }

  connection->cb = (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
  if (connection->cb == NULL) {
    system_error("malloc() failed");
    return NULL;
  }

  cb_init(connection->cb);
  connection->new_fd = new_fd;
  connection->next = NULL;
  connection->last_activity = time(NULL);

  return connection;
}

void insert_new_connection(host *host, int new_fd, char *buffer) {
  new_connection *connection = create_new_connection(new_fd);
  if (connection == NULL) {
    exit(1);
  }

  size_t len = strlen(buffer);
  if (cb_write(connection->cb, buffer, len) != len) {
    // !TODO - error handling
    free(connection->cb), free(connection);
    return;
  }

  connection->next = host->new_connections_list;
  host->new_connections_list = connection;
}

int remove_new_connection(host *host, int new_fd) {
  new_connection *temp = host->new_connections_list;
  new_connection *prev = NULL;

  if (temp != NULL && temp->new_fd == new_fd) {
    host->new_connections_list = temp->next;
    free(temp);
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
  free(temp);

  return 1;
}

void delete_new_connections_list(host *host) {
  new_connection *temp = NULL;

  while (host->new_connections_list != NULL) {
    temp = host->new_connections_list;
    host->new_connections_list = temp->next;
    free(temp->cb), free(temp);
  }

  host->new_connections_list = NULL;
}
