#include "../hdr/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  user_args *uip = NULL;
  uip = parser(argc, argv);

  free(uip);
  return EXIT_SUCCESS;
}
