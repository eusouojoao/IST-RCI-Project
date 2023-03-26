#ifndef DESCRIPTOR_CONTROL_H
#define DESCRIPTOR_CONTROL_H

#include "../common/struct.h"

#include <sys/select.h>

void update_working_set(host *host, fd_set *working_set);
int get_maxfd(host *host);
int wait_for_ready_fildes(host *host, fd_set *working_set, int *counter,
                          struct timeval *timeout);

int fildes_control(host *host, fd_set *working_set, int *counter, char *buffer);
int handle_keyboard_input(host *host, char *buffer);
int handle_new_connection(host *host, char *buffer);
int handle_queued_connections(host *host, fd_set *working_set, char *buffer);
int handle_neighbour_nodes(host *host, fd_set *working_set, char *buffer);

#endif
