#ifndef PARSER
#define PARSER

#include "struct.h"

void init_uip(user_input **uip);
int check_input_integrity(int argc, char *argv[], user_input **uip);
user_input *parser(int argc, char *argv[]);

#endif // !PARSER
