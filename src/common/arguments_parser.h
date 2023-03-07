#ifndef PARSER
#define PARSER

#include "struct.h"

void init_uip(user_args **uip);
int check_input_integrity(int argc, char *argv[], user_args **uip);
user_args *parser(int argc, char *argv[]);

#endif // !PARSER
