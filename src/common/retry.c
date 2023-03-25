#include "retry.h"

#include <stdio.h>
#include <sys/socket.h>
#include <time.h>

/**
 * @brief Delays the execution of the program based on the given attempt
 *
 * @param attempt The current attempt number. If negative, the delay will be shorter
 */
void delay(int attempt) {
  // Calculate the absolute value of the attempt
  unsigned int abs_attempt = (unsigned int)((attempt < 0) ? -attempt : attempt);
  // Calculate the delay time based on the attempt
  double result = (1 << abs_attempt) * (attempt < 0 ? 0.5 : 1.0);
  result *= 0.2;         // Scale down exponential delay to (0.2 * 2^attempt) seconds
  result *= 1000000000L; // Convertion from seconds to nanoseconds

  // Create a timespec struct for the desired delay in seconds
  struct timespec ts = {
      .tv_sec = 0,
      .tv_nsec = (long)result,
  };
  // Sleep for the calculated delay duration
  nanosleep(&ts, NULL);
}

int set_timeouts(int fd) {
  struct timeval timeout = {
      .tv_sec = TIMEOUT_SEC,
      .tv_usec = 0,
  };

  // Set the send timeout
  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt SO_SNDTIMEO");
    return -1;
  }

  // Set the receive timeout
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("setsockopt SO_RCVTIMEO");
    return -1;
  }

  return 0;
}
