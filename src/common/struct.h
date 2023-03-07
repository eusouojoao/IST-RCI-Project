#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct user_args {
  char *IP;
  int TCP;
  char *regIP;
  int regUDP;
} user_args;

typedef struct node {
  char *ID;
  int fd;
  char *IP;
  int TCP;

  struct node *next;
} node;

typedef struct names {
  char name[100];

  struct names *next;
} names;

typedef struct host {
  char *ID, *net;
  int tab_expedicao[100];
  user_args *uip;

  node *ext;       // pointer to ext node in the node_list
  node *bck;       // pointer to bck node (that is not in the node_list)
  node *node_list; // lista dos nós vizinhos, i.e., internos e externos

  names *names_list;
} host;

#endif // !STRUCTS_H