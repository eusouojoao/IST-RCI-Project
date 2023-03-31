/******************************************************************************
 *
 * File: prompts.h
 *
 * Description: This header file contains macros, functions and constants for
 *              managing and displaying prompts and user interface elements.
 *              It includes functionality to clear the terminal, print a prompt,
 *              and handle SIGQUIT signals (remapped to CTRL+C).
 *
 *****************************************************************************/
#ifndef PROMPTS_H
#define PROMPTS_H

#define ON 1
#define OFF 0

// Macro to clear the terminal screen
#define CLEAR_STREAM(S) write((S), "\033[H\033[J", 7)

// Macro to print a wrapper for a function (F)
#define PRINT_WRAPPER(F)                                                                      \
  do {                                                                                        \
    clear_stdout_line();                                                                      \
    (F);                                                                                      \
    prompt();                                                                                 \
  } while (0)

// Function prototypes
void print_char_n_times(char c, int n);
void print_header();
void user_interface_toggle(int toggle);
void clear_stdout_line();
void prompt();
void handle_sigquit();

#endif // PROMPTS_H
