#include "descriptor_control.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

int get_maxfd(host *host) {
  /* O primeiro elemento da lista de nós terá o maior file descriptor */
  return host->node_list == NULL ? host->listen_fd : host->node_list->fd;
}

void process_newfd(host *host, int new_fd, char *buffer) {
  char msg_to_send[SIZE << 2] = {'\0'}, cmd[SIZE] = {'\0'};
  char new_ID[SIZE] = {'\0'}, new_IP[SIZE] = {'\0'}, new_TCP[SIZE] = {'\0'};
  /*! TODO: ERROR control do input do nó que se conectou */
  if (sscanf(buffer, "%s %s %s %s", cmd, new_ID, new_IP, new_TCP) < 4) {
    /*error*/
    close(new_fd);
    return;
  }
  if (strcmp(cmd, "NEW") != 0) {
    /*error*/
    close(new_fd);
    return;
  }
  if (check_boot_parameters(new_ID, new_IP, new_TCP) == EXIT_FAILURE) {
    /*error*/
    close(new_fd);
    return;
  }

  if (host->ext == NULL) { // caso nós âncora
    sprintf(msg_to_send, "EXTERN %s %s %s", new_ID, new_IP, new_TCP);
    if (write(new_fd, msg_to_send, sizeof(msg_to_send)) == -1) {
      /*error*/;
    }
    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  } else { // caso normal
    sprintf(msg_to_send, "EXTERN %s %s %d", host->ext->ID, host->ext->IP, host->ext->TCP);
    if (write(new_fd, msg_to_send, sizeof(msg_to_send)) == -1) {
      /*error*/;
    }
    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  }

  return; // OK
}
