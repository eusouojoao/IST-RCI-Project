/******************************************************************************
 *
 * File: error_handling.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling errors and displaying usage information. It includes
 *              functions for displaying error messages, printing help
 *              information, and defining color codes for console output.
 *
 *****************************************************************************/
#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "../essentials/struct.h"

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

void usage(char *target);
void print_help(void);
void user_input_error(char *msg, char *input, char *detail);
void system_error(char *msg);
void die_with_system_error(host *host, char *msg);
void user_error(char *msg);
void print_usage();

#endif // ERROR_HANDLING_H
