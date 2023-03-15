#include "retry.h"

#include <stdarg.h>
#include <time.h>
#include <unistd.h>

void delay(int seconds) {
  struct timespec ts = {0, seconds * 1000000000L};
  nanosleep(&ts, NULL);
}

int retry(int (*function)(va_list), int max_attempts, ...) {
  int retval = -1;
  va_list args;
  va_start(args, max_attempts);

  for (int attempts = 0; attempts < max_attempts; attempts++) {
    retval = function(args);
    if (retval != -1) {
      return 0;
    }

    delay(attempts);
  }

  va_end(args);
  return -1; // return failure after max attempts
}
