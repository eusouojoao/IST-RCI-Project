/******************************************************************************
 *
 * File: UDP_utils.h
 *
 * Description: This header file contains the declarations of functions for
 *              handling UDP sockets, such as sending and receiving messages,
 *              and retrying with exponential backoff.
 *
 *****************************************************************************/
#ifndef UDP_H
#define UDP_H

#include "../../essentials/struct.h"

#ifndef RETRY_MACRO
#define MAX_ATTEMPTS 10

/**
 * @brief Retries a given function up to a maximum number of attempts with an exponential
 * backoff.
 *
 * This macro takes a function (F) and a maximum number of attempts (max_attempts),
 * then executes the function repeatedly until it succeeds or the maximum number
 * of attempts is reached. The result of the function is stored in the provided
 * result_var variable.
 *
 * @note MAX_ATTEMPTS indicates the maximum number of attempts to retry the function.
 *
 * @param F: the function to be retried. This can be a function call with
 * arguments.
 * @param result_var: the variable that will store the result of the function
 * call.
 */
#define RETRY(F, result_var)                                                                  \
  do {                                                                                        \
    /* Initialize the retry count to zero */                                                  \
    int attempt = 0;                                                                          \
                                                                                              \
    /* Initialize the result variable to -1 */                                                \
    (result_var) = -1;                                                                        \
                                                                                              \
    /* Loop while the retry count is less than the maximum number of attempts */              \
    while (attempt < (MAX_ATTEMPTS)) {                                                        \
      /* Call the function passed as the F argument and store the result */                   \
      (result_var) = (F);                                                                     \
                                                                                              \
      /* Check if the result is not equal to -1 (i.e., no error occurred) */                  \
      if ((result_var) != -1) {                                                               \
        /* If so, break out of the loop */                                                    \
        break;                                                                                \
      }                                                                                       \
                                                                                              \
      if (errno == ENETUNREACH || errno == EWOULDBLOCK || errno == EAGAIN) {                  \
        fputs("[UDP] Retrying...\n", stdout);                                                 \
        fflush(stdout);                                                                       \
        /* Call the delay function, passing the current retry count as an argument */         \
        if (attempt <= 5) {                                                                   \
          delay(attempt);                                                                     \
        } else {                                                                              \
          sleep(10);                                                                          \
        }                                                                                     \
        /* Increment the retry count */                                                       \
        attempt++;                                                                            \
      } else {                                                                                \
        /* Another unexpected error occurred */                                               \
        break;                                                                                \
      }                                                                                       \
    }                                                                                         \
  } while (0)
#endif // RETRY_MACRO

char *send_and_receive_msg_UDP(user_args *uip, char *msg);

#endif // UDP_H
