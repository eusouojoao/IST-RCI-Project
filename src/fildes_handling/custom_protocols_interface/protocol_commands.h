/******************************************************************************
 *
 * File: protocol_commands.h
 *
 * Description: This header file contains the enumeration of protocol commands
 *              used in the system.
 *
 *****************************************************************************/
#ifndef PROTOCOL_COMMANDS_H
#define PROTOCOL_COMMANDS_H

typedef enum protocol_command {
  EXTERN,
  WITHDRAW,
  QUERY,
  CONTENT,
  NOCONTENT,
  BADFORMAT
} protocol_command;

#endif // !PROTOCOL_COMMANDS_H
