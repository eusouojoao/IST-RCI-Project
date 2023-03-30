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
  fputs(RESET "\n", stderr);
}

/**
 * @brief  Displays a simple user input error message
 * @note   Print in the format fprintf
 * @param  msg: pointer to the main error message
 */
void user_error(char *msg) { fprintf(stderr, RED "(!) Error:" RESET " %s\n", msg); }

/* clang-format off */

/**
 * @brief  Print help command to inform user of the commands of the program
 */
void print_help(void) {
  printf("╔═══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
  printf("║                                    User Interface Commands                                    ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `join`: [attempts to connect the host to the specified network]                             ║\n");
  printf("║    # args: 'NET ID'                                                                           ║\n");
  printf("║ - `djoin`: [connects the host to a network without communication with the node server]        ║\n");
  printf("║    # args: 'NET ID bootID bootIP bootTCP'                                                     ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `create`: [creates a new content]                                                           ║\n");
  printf("║    # args: 'name' {alphanumeric name, '.' character is an expection}                          ║\n");
  printf("║ - `delete`: [deletes a specific content]                                                      ║\n");
  printf("║    # args: 'name' {alphanumeric name, '.' character is an expection}                          ║\n");
  printf("║ - `get`: [searches for a specific content]                                                    ║\n");
  printf("║    # args: 'dest name'                                                                        ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `show topology` (st): [shows the ID and contacts of all neighbour nodes and backup node]    ║\n");
  printf("║ - `show names` (sn): [shows all contents]                                                     ║\n");
  printf("║ - `show routing` (sr): [shows all known routes]                                               ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `clear names` (cn): [deletes all created content]                                           ║\n");
  printf("║ - `clear routing` (cr): [clears the forwarding table]                                         ║\n");
  printf("║ - `clear window` (cw) (CTRL+L): [clears the terminal window]                                  ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `leave`: [leave the current network]                                                        ║\n");
  printf("║ - `exit`: [terminates the program]                                                            ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - `help` (?): [prints the user manual]                                                        ║\n");
  printf("╚═══════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}
