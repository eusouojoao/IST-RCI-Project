#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct user_args
{
  char *IP;
  int TCP;
  char *regIP;
  int regUDP;
} user_args;

typedef struct node
{
  int ID;
  int fd;
  char *IP;
  int TCP;

  struct node *next;
} node;
typedef struct names
{
  char name[100];

  struct names *next;
} names;

typedef struct host
{
  int ID;
  int tab_expedicao[100];
  user_args *uip;

  node *ext;
  node *bck;
  node *node_list; // lista dos nós vizinhos, i.e., internos e externos

  names *names_list;
} host;

#endif // !STRUCTS_H
