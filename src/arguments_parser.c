#include "arguments_parser.h"
#include "error_handling/error_checking.h"
#include "error_handling/error_messages.h"

#include <stdlib.h>

/**
 * @brief Initializes a dynamically allocated user_args structure.
 *
 * Initializes a user_args structure with default values to facilitate subsequent user input
 * evaluation (will be overwritten if the parameters pass the next evaluation stage).
 *
 * @param uip: pointer to the user_args structure to be initialized.
 */
void init_uip(user_args **uip) {
  (*uip)->IP = "NULL";
  (*uip)->TCP = 0;
  (*uip)->regIP = "193.136.138.142";
  (*uip)->regUDP = 59000;
}

/**
 * @brief Checks the integrity of user-supplied input arguments.
 *
 * Verifies if the given command-line arguments are valid (IP, TCP, regIP, regTCP) and assigns
 * them to the user_args structure if they pass validation.
 *
 * @param argc: number of command-line arguments
 * @param argv: array of command-line arguments
 * @param uip: pointer to the user_args structure to be updated with validated
 * arguments
 *
 * @return 1 if the input arguments are valid, 0 if not.
 */
int check_input_integrity(int argc, char *argv[], user_args **uip) {

  if (!check_IP_address(argv[1])) {
    return 0;
  }

  (*uip)->IP = argv[1];              // Assign the validated IP
  (*uip)->TCP = check_PORT(argv[2]); // Assign the validated port
  if ((*uip)->TCP == -1) {
    return 0;
  }

  if (argc == 5) {
    if (!check_IP_address(argv[3])) {
      return 0;
    }

    (*uip)->regIP = argv[3];              // Assign the validated IP
    (*uip)->regUDP = check_PORT(argv[4]); // Assign the validated port
    if ((*uip)->regUDP == -1) {
      return 0;
    }
  }

  return 1;
}

/**
 * @brief Parses user input arguments to initialize the user_args structure.
 *
 * @param argc: number of command-line arguments
 * @param argv: array of command-line arguments
 *
 * @return pointer to the user_args structure with the parsed arguments.
 */
user_args *arguments_parser(int argc, char *argv[]) {
  int input_ok = 0;

  // Check if the number of user-supplied parameters violates the specified requirements
  if (argc != 3 && argc != 5) {
    usage(argv[0]);
    exit(1);
  }

  // Initialize the user_args structure
  user_args *uip = calloc(1, sizeof(user_args));
  if (uip == NULL) {
    die_with_system_error(NULL, "calloc() failed");
  }
  init_uip(&uip);

  // Verify the integrity of user-supplied parameters (IP, TCP, regIP, regTCP),
  // and assign the values to the user_args structure if valid
  input_ok = check_input_integrity(argc, argv, &uip);
  if (!input_ok) {
    usage(argv[0]);
    free(uip);
    exit(1);
  }

  return uip;
}
