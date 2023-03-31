#include "error_messages.h"
#include "../fildes_handling/user_interface/leave_module.h"
#include "../misc/prompts.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
 * @brief Terminates the program after displaying a system error message.
 *
 * This function calls the system_error() function to display a system error
 * message, including the error message associated with the current value of
 * errno. After displaying the error message, it clears the host and exits
 * the program with a non-zero status code.
 *
 * @param host: pointer to the host structure to be cleared
 * @param msg: pointer to the main error message
 */
void die_with_system_error(host *host, char *msg) {
  // Call the system_error() function to display the error message
  if (msg != NULL) {
    system_error(msg);
  }

  // Clear the host structure
  if (host != NULL) {
    clear_host(host), delete_names_list(host);
    close(host->listen_fd);
    free(host->uip);
    free(host);
  }

  user_interface_toggle(OFF);
  // Terminate the program with a non-zero status code
  exit(1);
}

/**
 * @brief  Displays a simple user input error message
 * @note   Print in the format fprintf
 * @param  msg: pointer to the main error message
 */
void user_error(char *msg) { fprintf(stderr, RED "(!) Error:" RESET " %s\n", msg); }

/* clang-format off */

/**
 * @brief Print help command to inform user of the commands of the program
 */
void print_help(void) {
  printf("╔═══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
  printf("║                                    User Interface Commands                                    ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`join`"RESET": [attempts to connect the host to the specified network]                             ║\n");
  printf("║    # args: 'NET' {000-999} 'ID' {00-99}                                                       ║\n");
  printf("║    - e.g. "YELLOW"join 000 00"RESET"                                                                         ║\n");
  printf("║ - "RED"`djoin`"RESET": [connects the host to a network without communication with the node server]        ║\n");
  printf("║    # args: 'NET' {000-999} 'ID' {00-99} 'bootID' {00-99} 'bootIP' {IPv4} 'bootTCP' {0-65535}  ║\n");
  printf("║    - e.g. "YELLOW"djoin 000 01 00 127.0.0.1 59000"RESET"                                                     ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`create`"RESET": [creates a new content]                                                           ║\n");
  printf("║    # args: 'name' {alphanumeric name, '.' character is an exception}                          ║\n");
  printf("║    - e.g. "YELLOW"create file.txt"RESET"                                                                     ║\n");
  printf("║ - "RED"`delete`"RESET": [deletes a specific content]                                                      ║\n");
  printf("║    # args: 'name' {alphanumeric name, '.' character is an exception}                          ║\n");
  printf("║    - e.g. "YELLOW"delete file.txt"RESET"                                                                     ║\n");
  printf("║ - "RED"`get`"RESET": [searches for a specific content]                                                    ║\n");
  printf("║    # args: 'dest' {00-99} 'name' {alphanumeric name, '.' character is an exception}           ║\n");
  printf("║    - e.g. "YELLOW"get 00 file.txt"RESET"                                                                     ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`show topology`"RESET" (st): [shows the ID and contacts of all neighbour nodes and backup node]    ║\n");
  printf("║ - "RED"`show names`"RESET" (sn): [shows all contents]                                                     ║\n");
  printf("║ - "RED"`show routing`"RESET" (sr): [shows all known routes]                                               ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`clear names`"RESET" (cn): [deletes all created content]                                           ║\n");
  printf("║ - "RED"`clear routing`"RESET" (cr): [clears the forwarding table]                                         ║\n");
  printf("║ - "RED"`clear window`"RESET" (cw) (CTRL+L): [clears the terminal window]                                  ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`leave`"RESET": [leave the current network]                                                        ║\n");
  printf("║ - "RED"`exit`"RESET": [terminates the program]                                                            ║\n");
  printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
  printf("║ - "RED"`help`"RESET" (?): [prints the user manual]                                                        ║\n");
  printf("╚═══════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}
