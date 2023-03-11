
#ifndef PROTOCOl_COMMANDS_H
#define PROTOCOl_COMMANDS_H

typedef enum protocol_command {
  EXTERN,
  WITHDRAW,
  QUERY,
  CONTENT,
  NOCONTENT,
  BADFORMAT
} protocol_command;

#endif // !PROTOCOL_COMMANDS_H
