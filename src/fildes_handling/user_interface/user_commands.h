#ifndef USER_COMMANDS_H
#define USER_COMMANDS_H

typedef enum user_command {
  JOIN,
  DJOIN,
  CREATE,
  DELETE,
  GET,
  SHOW,
  SHOW_TOPOLOGY,
  SHOW_NAMES,
  SHOW_ROUTING,
  LEAVE,
  EXIT,
  CLEAR,
  CLEAR_ROUTING,
  CLEAR_NAMES,
  CLEAR_WINDOW,
  UNDEF
} user_command;

#endif // !USER_COMMANDS_H
