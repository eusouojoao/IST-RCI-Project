/******************************************************************************
 *
 * File: error_checking.h
 *
 * Description: This header file contains the declarations of functions for
 *              checking the validity of various input parameters such as
 *              node IDs, IP addresses, port numbers, and names. It is used to
 *              ensure the consistency and correctness of the data.
 *
 *****************************************************************************/
#ifndef ERROR_CHECKING_H
#define ERROR_CHECKING_H

int number_of_command_arguments(char *str, char delim);
int check_node_parameters(char *node_ID, char *node_IP, char *node_TCP);
int check_id(char *id);
int check_net(char *net);
int check_net_and_id(char *net, char *id);
int check_if_number(char *src);
int check_IP_address(char *src);
int check_PORT(char *src);
int check_name(char *name);

#endif // ERROR_CHECKING_H
