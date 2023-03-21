#include "prompts.h"
#include "../error_handling/error_messages.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

void print_char_n_times(char c, int n) {
  for (int i = 0; i < n; i++) {
    putchar(c);
  }
}

void print_header() {
  struct winsize w;
  // Get terminal width
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col <= 0) {
    fprintf(stderr, "Error getting terminal width\n");
    return;
  }

  // If terminal width is less than 60, do nothing
  if (w.ws_col < 60) {
    return;
  }

  CLEAR_STREAM(STDOUT_FILENO);
  printf("%*s", (w.ws_col - 60) / 2, "");
  putchar('/'), print_char_n_times('-', 58), putchar('\\'), putchar('\n');
  printf("%*s", (w.ws_col - 60) / 2, "");
  printf("| %-56s |\n", "[Redes de Computadores e Internet] 2o Semestre 2022/2023");
  printf("%*s", (w.ws_col - 60) / 2, "");
  printf("| %-56s |\n", "   -> Transferência de Conteúdos em Redes Arbóreas <-   ");
  printf("%*s", (w.ws_col - 60) / 2, "");
  printf("| %-56s |\n", "       Daniel Dinis 99906 -- João Gonçalves 99995       ");
  printf("%*s", (w.ws_col - 60) / 2, "");
  putchar('\\'), print_char_n_times('-', 58), putchar('/'), putchar('\n');
}

void user_interface_toggle(int toggle) {
  struct winsize w;
  // Get terminal width
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col <= 0) {
    fprintf(stderr, "Error getting terminal width\n");
    return;
  }

  // If terminal width is less than 60, do nothing
  if (w.ws_col < 20) {
    return;
  }

  printf("%*s", (w.ws_col - 20) / 2, "");
  if (toggle == ON) {
    printf(BLUE " User interface [" GREEN "ON" BLUE "]\n" RESET);
  } else if (toggle == OFF) {
    printf(BLUE " User interface [" RED "OFF" BLUE "]\n" RESET);
  }
}

void clear_stdout_line() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); // Get the terminal size

  // Move the cursor to the beginning of the line
  printf("\r");
  // Overwrite the line with spaces
  print_char_n_times(' ', w.ws_col);
  // Move the cursor back to the beginning of the line
  printf("\r");
}

void prompt() {
  clear_stdout_line();
  printf(GREEN "<USER> " RESET);
  fflush(stdout);
}

void handle_sigquit() {
  CLEAR_STREAM(STDOUT_FILENO);
  prompt();
}
