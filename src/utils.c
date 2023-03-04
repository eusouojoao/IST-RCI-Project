#include <stdlib.h>
#include <string.h>
#include "../hdr/utils.h"
/*
user_args *argsInit(char *IP, int TCP, char *regIP, int regUDP)
{
    user_args *newArg = (user_args *)malloc(sizeof(struct user_args));
    if (newArg == NULL)
        exit(0);
    newArg->IP = (char *)malloc((strlen(IP) + 1) * sizeof(char));
    if (newArg->IP == NULL)
        exit(0);
    strcpy(IP, newArg->IP);
    newArg->TCP = TCP;
    newArg->regIP = (char *)malloc((strlen(regIP) + 1) * sizeof(char));
    if (newArg->regIP == NULL)
        exit(0);
    strcpy(regIP, newArg->regIP);
    newArg->regUDP = regUDP;
    return newArg;
}
*/

host *hostInit(int ID, user_args *uip)
{
    host *newH = (host *)malloc(sizeof(struct host));
    if (newH == NULL)
        exit(0);
    newH->ID = ID;
    newH->uip = uip;
    newH->bck = NULL; //NULL representa que o proprio nó é o backup
    newH->ext = NULL; // ... e vizinho externo
    newH->node_list = NULL;
    newH->names_list = NULL;
    for (int i = 0; i < 100; i++)
    {
        newH->tab_expedicao[i] = -1; //-1 representa sem informação do vizinho ao nó destino i
    }
    return newH;
}
node *newnode(int ID, int fd, char *IP, int TCP, node *next)
{
    node *newN = (node *)malloc(sizeof(struct node));
    if (newN == NULL)
        exit(0);
    newN->ID = ID;
    newN->fd = fd;
    newN->TCP = TCP;
    newN->IP = (char *)malloc((strlen(IP) + 1) * sizeof(char));
    if (newN->IP == NULL)
        exit(0);
    strcpy(IP, newN->IP);
    newN->next = next; //novo node passa a ser o 1º da lista
    return newN;
}
