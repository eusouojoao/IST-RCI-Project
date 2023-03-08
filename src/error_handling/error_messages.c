#include "error_messages.h"

#include <stdio.h>

void usage(char *target) {
  fprintf(stderr, RED "(!) Usage:" RESET BLUE " %s IP TCP [regIP] [regTCP]\n\n" RESET,
          target);
  fprintf(stderr, BLUE ">NOTE:" RESET
                       " The arguments [regIP] & [regTCP] are optional, defaulting\n"
                       "to 193.136.138.142 & 59000 respectively.\n");
  return;
}

void UDP_server_message(int error_flag, char *msg) {
  if (error_flag) {
    fputs(RED "(!) Server reply:\n" RESET, stderr);
    fputs(msg, stderr);
    fputs("\n", stderr);
    return;
  }
  fputs(YELLOW "(OK) Server reply:\n" RESET, stderr);
  fputs(msg, stderr);
  fputs("\n", stderr);
}

void user_input_error(char *msg, char *input, char *detail) {
  fputs(RED "(!) " RESET, stderr);
  fputs(msg, stderr);
  fputs(": ", stderr);
  fputs(RED, stderr);
  fputs(input, stderr);
  fputs(RESET, stderr);
  fputs(" -> ", stderr);
  fputs(detail, stderr);
  fputs("\n", stderr);
}

void system_error(char *msg) {
  fputs(RED "(!) " RESET, stderr);
  perror(msg);
}
