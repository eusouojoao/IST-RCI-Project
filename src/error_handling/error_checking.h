#ifndef ERROR_CHECKING_H
#define ERROR_CHECKING_H

int number_of_command_arguments(char *str, char delim);
int check_boot_parameters(char *bootID, char *bootIP, char *bootTCP);
int check_net_and_id(char *net, char *id);
int check_if_number(char *src);
int check_IP_address(char *src);
int check_PORT(char *src);

#endif // !ERROR_CHECKING_H
