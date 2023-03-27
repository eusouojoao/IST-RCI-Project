#include "error_messages.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Displays the usage instructions for the program.
 *
 * This function formats and prints the usage instructions for the target
 * program to the standard error stream, including the program's expected
 * arguments and default values.
 *
 * @param target: pointer to the program's target name
 */
void usage(char *target) {
  fprintf(stderr, RED "(!) Usage:" RESET BLUE " %s IP TCP [regIP] [regTCP]\n\n" RESET, target);
  fputs(BLUE "NOTE:" RESET " The arguments [regIP] & [regTCP] are optional, defaulting\n"
             "to 193.136.138.142 & 59000 respectively.\n",
        stderr);
}

/**
 * @brief Displays a user input error message.
 *
 * This function formats and prints a user input error message to the standard
 * error stream, along with an error detail message.
 *
 * @param msg: pointer to the main error message
 * @param input: pointer to the input that caused the error
 * @param detail: pointer to the error detail message
 */
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

/**
 * @brief Displays a system error message.
 *
 * This function formats and prints a system error message to the standard
 * error stream, including the error message associated with the current
 * value of errno.
 *
 * @param msg: pointer to the main error message
 */
void system_error(char *msg) {
  fputs(RED "(!) " RESET, stderr);
  fputs(msg, stderr);
  fputs(": ", stderr);
  fputs(RED, stderr);
  fputs(strerror(errno), stderr);
  fputs(RESET, stderr);
}

void user_error(char *msg) { fprintf(stderr, RED "(!) Error" RESET "%s", msg); }

void print_help(void) {
  printf("╔══════════════════════════════════════════════════════════════════════════════"
         "═════════════════╗\n");
  printf("║                                    User Interface Commands                   "
         "                 ║\n");
  printf("╠══════════════════════════════════════════════════════════════════════════════"
         "═════════════════╣\n");
  printf("║ - join net id : [adds a node to the specify net]                             "
         "                 ║\n");
  printf("║ - djoin net id bootid bootIP bootTCP : [adds a node without communication "
         "with the server]    ║\n");
  printf("║ - create name : [creates a new content]                                      "
         "                 ║\n");
  printf("║ - delete name : [deletes a specific content]                                 "
         "                 ║\n");
  printf("║ - get dest name : [searches for a specific content]                          "
         "                 ║\n");
  printf("║ - show topology (st) : [shows ID and contact of neighbours and backup nodes] "
         "                 ║\n");
  printf("║ - show names (sn) : [shows all contents]                                     "
         "                 ║\n");
  printf("║ - show routing (sr) : [shows all known routes]                               "
         "                 ║\n");
  printf("║ - clear names (cn) : [deletes all contents]                                  "
         "                 ║\n");
  printf("║ - clear routing (cr) : [clears rote table]                                   "
         "                 ║\n");
  printf("║ - clear window (cw) : [clears terminal window]                               "
         "                 ║\n");
  printf("║ - leave : [node leaves net]                                                  "
         "                 ║\n");
  printf("║ - exit : [closes app]                                                             "
         "            ║\n");
  printf("║ - help : [shows User Interface Commands]                                          "
         "            ║\n");

  printf("╚══════════════════════════════════════════════════════════════════════════════"
         "═════════════════╝\n");

  printf("Type 'help' or '?' for more information.\n"); // APAGAR - acho q n é preciso
}
