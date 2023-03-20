#include "arguments_parser.h"
#include "../error_handling/error_checking.h"
#include "../error_handling/error_messages.h"

#include <stdlib.h>

/**
 * @brief Initializes a dynamically allocated user_args structure.
 *
 * Initializes a user_args structure with default values to facilitate
 * subsequent user input evaluation (will be overwritten if the parameters
 * pass the next evaluation stage).
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
 * Verifies if the given command-line arguments are valid (IP, TCP, regIP, regTCP)
 * and assigns them to the user_args structure if they pass validation.
 *
 * @param argc: number of command-line arguments
 * @param argv: array of command-line arguments
 * @param uip: pointer to the user_args structure to be updated with validated
 * arguments
 *
 * @return (int) EXIT_SUCCESS if the input arguments are valid.
 */
int check_input_integrity(int argc, char *argv[], user_args **uip) {

  if (check_IP_address(argv[1]) != 1) {
    user_input_error("Invalid IPv4", argv[1],
                     "the IP format must be X.X.X.X, where X must be a "
                     "decimal value between 0 and 255 (octet).");
  }
  (*uip)->IP = argv[1];              // Assign the validated IP
  (*uip)->TCP = check_PORT(argv[2]); // Assign the validated port

  if (argc == 5) {
    if (check_IP_address(argv[3]) != 1) {
      user_input_error("Invalid IPv4", argv[3],
                       "the IP format must be X.X.X.X, where X must be a "
                       "decimal value between 0 and 255 (octet).");
    }
    (*uip)->regIP = argv[3];              // Assign the validated IP
    (*uip)->regUDP = check_PORT(argv[4]); // Assign the validated port
  }

  return EXIT_SUCCESS;
}

/**
 * @brief Parses user input arguments to initialize the user_args structure.
 *
 * @param argc: number of command-line arguments
 * @param argv: array of command-line arguments
 *
 * @return (user_args *) pointer to the user_args structure with the parsed arguments.
 */
user_args *arguments_parser(int argc, char *argv[]) {
  int input_ok = 0;

  // Check if the number of user-supplied parameters violates the specified requirements
  if (argc != 3 && argc != 5) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initialize the user_args structure
  user_args *uip = calloc(1, sizeof(user_args));
  if (uip == NULL) {
    system_error("calloc() failed: ");
    exit(EXIT_FAILURE);
  }
  init_uip(&uip);

  // Verify the integrity of user-supplied parameters (IP, TCP, regIP, regTCP),
  // and assign the values to the user_args structure if valid
  input_ok = check_input_integrity(argc, argv, &uip);
  if (input_ok != EXIT_SUCCESS) {
    usage(argv[0]);
    free(uip);
    exit(EXIT_FAILURE);
  }

  return uip;
}
