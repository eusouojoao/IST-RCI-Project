/******************************************************************************
 *
 * File: TCP_utils.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling TCP sockets, such as writing messages, fetching
 *              messages, and creating a listening socket.
 *
 *****************************************************************************/
#ifndef TCP_H
#define TCP_H

#include "../../essentials/struct.h"
#include <stdio.h>

ssize_t write_msg_TCP(int fd, char *msg_to_send, size_t msglen);
ssize_t fetch_bck(host *host, char *msg);
int create_listen_socket(user_args *uip);

#endif // TCP_H
