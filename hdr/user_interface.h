#ifndef user_interface_H
#define user_interface_H

#include "struct.h"

typedef enum user_command {
  JOIN,
  DJOIN,
  CREATE,
  DELETE,
  GET,
  SHOW,
  LEAVE,
  EXIT,
  UNDEF
} user_command;

void join(char *buffer, host **host);
void djoin(char *buffer, host **host);
void process_stdin_input(char *buffer, host **host);

#endif // !user_interface_H
