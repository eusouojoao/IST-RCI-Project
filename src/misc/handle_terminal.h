#ifndef HANDLE_TERMINAL_H
#define HANDLE_TERMINAL_H

#include <termios.h>

void modify_termios(struct termios *original_termios);
void restore_termios(const struct termios *original_termios);

#endif
