#ifndef error_handling_H
#define error_handling_H

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

void usage(char *target);
void print_help(void);
void user_input_error(char *msg, char *input, char *detail);
void system_error(char *msg);

#endif // !error_handling_H
