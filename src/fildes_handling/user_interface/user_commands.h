#ifndef USER_COMMANDS_H
#define USER_COMMANDS_H

typedef enum user_command {
  JOIN,
  DJOIN,
  CREATE,
  DELETE,
  GET,
  SHOW,
  LEAVE,
  EXIT,
  CLEAR,
  UNDEF
} user_command;

#endif // !USER_COMMANDS_H