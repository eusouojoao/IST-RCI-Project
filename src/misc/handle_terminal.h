/******************************************************************************
 *
 * File: handle_terminal.h
 *
 * Description: This header file contains functions for handling terminal
 *              settings, specifically modifying and restoring termios
 *              structures. It provides a way to temporarily change terminal
 *              settings and restore them to their original state.
 *
 *****************************************************************************/
#ifndef HANDLE_TERMINAL_H
#define HANDLE_TERMINAL_H

#include <termios.h>

// Function prototypes
void modify_termios(struct termios *original_termios);
void restore_termios(const struct termios *original_termios);

#endif
