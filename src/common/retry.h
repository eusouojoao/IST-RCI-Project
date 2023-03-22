#ifndef RETRY_H
#define RETRY_H

#define MAX_ATTEMPTS 5
#define TIMEOUT_SEC 5

void delay(int seconds);
int set_timeouts(int fd);

#ifndef RETRY_MACRO
/**
 * @brief Retries a given function up to a maximum number of attempts.
 *
 * This macro takes a function (F) and a maximum number of attempts (max_attempts),
 * then executes the function repeatedly until it succeeds or the maximum number
 * of attempts is reached. The result of the function is stored in the provided
 * result_var variable.
 *
 * @param[in] F: The function to be retried. This can be a function call with
 * arguments.
 * @param[in] max_attempts: The maximum number of attempts to retry the function.
 * @param[in,out] result_var: The variable that will store the result of the function
 * call.
 */
#define RETRY(F, max_attempts, result_var)                                               \
  do {                                                                                   \
    /* Initialize the retry count to zero */                                             \
    int attempt = 0;                                                                     \
                                                                                         \
    /* Initialize the result variable to -1 */                                           \
    (result_var) = -1;                                                                   \
                                                                                         \
    /* Loop while the retry count is less than the maximum number of attempts */         \
    while (attempt < (max_attempts)) {                                                   \
      /* Call the function passed as the F argument and store the result */              \
      (result_var) = (F);                                                                \
                                                                                         \
      /* Check if the result is not equal to -1 (i.e., no error occurred) */             \
      if ((result_var) != -1) {                                                          \
        /* If so, break out of the loop */                                               \
        break;                                                                           \
      }                                                                                  \
                                                                                         \
      fputs("Retrying...", stdout);                                                      \
                                                                                         \
      /* Call the delay function, passing the current retry count as an argument */      \
      delay(attempt);                                                                    \
      /* Increment the retry count */                                                    \
      attempt++;                                                                         \
    }                                                                                    \
  } while (0)
#endif // RETRY_MACRO

#endif
