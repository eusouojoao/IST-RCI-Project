#include "socket_descriptor_control.h"
#include "../common/utils.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"
#include "protocol_commands.h"
#include "withdraw_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

void update_working_set(host *host, fd_set *working_set) {
  FD_ZERO(working_set);
  FD_SET(STDIN_FILENO, working_set);
  FD_SET(host->listen_fd, working_set);
  for (node *temp = host->node_list; temp != NULL; temp = temp->next) {
    FD_SET(temp->fd, working_set);
  }
}

int get_maxfd(host *host) {
  /* O primeiro elemento da lista de nós terá o maior file descriptor */
  return host->node_list == NULL ? host->listen_fd : host->node_list->fd;
}

void process_new_fd(host *host, int new_fd, char *buffer) {
  char msg_to_send[SIZE << 2] = {'\0'}, cmd[SIZE] = {'\0'};
  char new_ID[SIZE] = {'\0'}, new_IP[SIZE] = {'\0'}, new_TCP[SIZE] = {'\0'};

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
  if (check_node_parameters(new_ID, new_IP, new_TCP) == EXIT_FAILURE) {
    /*error*/
    close(new_fd);
    return;
  }

  if (host->ext == NULL) { // caso nós âncora
    sprintf(msg_to_send, "EXTERN %s %s %s\n", new_ID, new_IP, new_TCP);
    if (write(new_fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      /*error*/;
      close(new_fd);
      return;
    }

    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  } else { // caso normal
    sprintf(msg_to_send, "EXTERN %s %s %d\n", host->ext->ID, host->ext->IP,
            host->ext->TCP);
    if (write(new_fd, msg_to_send, strlen(msg_to_send) + 1) == -1) {
      /*error*/;
      close(new_fd);
      return;
    }

    insert_node(new_ID, new_fd, new_IP, atoi(new_TCP), host);
  }

  return; // OK
}

protocol_command get_protocol_command(char *token) {
  if (strcmp(token, "EXTERN") == 0) {
    return EXTERN;
  } else if (strcmp(token, "WITHDRAW") == 0) {
    return WITHDRAW;
  } else if (strcmp(token, "QUERY") == 0) {
    return QUERY;
  } else if (strcmp(token, "CONTENT") == 0) {
    return CONTENT;
  } else if (strcmp(token, "NOCONTENT") == 0) {
    return NOCONTENT;
  }

  return BADFORMAT;
}

void process_neighbour_node_fd(host *host, node *node, char *buffer) {
  char token[32] = {'\0'}, ID[32] = {'\0'};
  printf("buffer: %s\n", buffer);
  if (sscanf(buffer, "%s", token) < 1) {
    system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
  }

  switch (get_protocol_command(token)) {
  case EXTERN:
    update_backup(host, buffer);
    break;
  case WITHDRAW:
    if (sscanf(buffer, "WITHDRAW %s", ID) < 1) {
      system_error("In process_neighbour_node_fd() ->" RED " sscanf() failed");
      return;
    }
    if (!(strlen(ID) == 2 && check_if_number(ID))) {
      /* error, protocol with bad format */
      printf("Error! Bad format!\n");
      return;
    }
    withdraw_node(host, ID);
    break;
  case QUERY:
    break;
  case CONTENT:
    break;
  case NOCONTENT:
    break;
  case BADFORMAT:
  default:
    /*error*/
    printf("Something something bad format from NODE: %s\n", node->ID);
    break;
  }

  return;
}
