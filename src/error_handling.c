#include <stdio.h>
#include <stdlib.h>

#include "../hdr/error_handling.h"

#define clear() fprintf(stderr, "\033[H\033[J")
#define RED "\x1B[31m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

void usage(char *target) {
  fprintf(stderr,
          RED "(!) Usage:" RESET BLUE " %s IP TCP [regIP] [regTCP]\n\n" RESET,
          target);
  fprintf(stderr,
          BLUE ">NOTE:" RESET
               " The arguments [regIP] & [regTCP] are optional, defaulting\n"
               "to 193.136.138.142 & 59000 respectively.\n");
  return;
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
  exit(EXIT_FAILURE);
}

void system_error(char *msg) {
  fputs(RED "(!) " RESET, stderr);
  perror(msg);
  exit(EXIT_FAILURE);
}
