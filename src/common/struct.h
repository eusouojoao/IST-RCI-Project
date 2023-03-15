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
  char *name;

  struct names *next;
} names;

typedef struct host {
  char *ID, *net;
  int tab_expedicao[100];
  user_args *uip;

  int listen_fd;

  node *bck;       // pointer to bck node (not in the node_list)
  node *ext;       // pointer to ext node (also available in the node_list)
  node *node_list; // list of neighbour nodes, i.e., internal and external

  names *names_list;
} host;

#endif // !STRUCTS_H
