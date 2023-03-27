#include "handle_terminal.h"

#include <unistd.h>

void modify_termios(struct termios *original_termios) {
  struct termios new_termios;

  // Get the current terminal settings
  tcgetattr(STDIN_FILENO, original_termios);
  new_termios = *original_termios;

  // Set the VQUIT character to CTRL+L (0x0C)
  new_termios.c_cc[VQUIT] = 0x0C;

  // Apply the new terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restore_termios(const struct termios *original_termios) {
  // Restore the original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, original_termios);
}
