/******************************************************************************
 *
 * File: arguments_parser.h
 *
 * Description: This header file contains the declarations of functions for
 *              parsing and validating command line arguments. It also
 *              includes the required header file for the user_args structure.
 *
 *****************************************************************************/
#ifndef ARGUMENTS_PARSER_H
#define ARGUMENTS_PARSER_H

#include "essentials/struct.h"

// Initializes a user_args structure with default values
void init_uip(user_args **uip);

// Checks the integrity of the input arguments and fills the user_args structure
// Returns 0 on success, -1 on error
int check_input_integrity(int argc, char *argv[], user_args **uip);

// Parses the command line arguments and returns a user_args structure
user_args *arguments_parser(int argc, char *argv[]);

#endif // ARGUMENTS_PARSER_H
