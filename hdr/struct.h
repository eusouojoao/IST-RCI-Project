#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
  char *IP;
  int TCP;
  char *regIP;
  int regUDP;
} user_args;

typedef struct node {
  int ID;
  int fd;
  char *IP;
  int TCP;

  struct node *next;
} node;

typedef struct {
  user_args* uip;
  int UDP_fd;

  node *ext;
  node *bck;
  node *node_list; // lista dos nós vizinhos, i.e., internos e externos

} host;

#endif // !STRUCTS_H
