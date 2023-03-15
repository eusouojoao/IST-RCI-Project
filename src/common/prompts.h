#ifndef PROMPTS_H
#define PROMPTS_H

#define ON 1
#define OFF 0

void print_char_n_times(char c, int n);
void print_header();
void user_interface_toggle(int toggle);
void prompt();
void handle_sigquit();

#endif
