#include "../hdr/UDP_utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void send_user_message_UDP(user_args *uip, char *msg) {
  int fd = -1;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  if (inet_pton(AF_INET, uip->regIP, &(addr.sin_addr)) != 1) {
    /*error*/ exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons((in_port_t)uip->regUDP);

  // sendto(...);

  /*! TODO: Tratamento de timeout, caso a mensagem se perca, ou o servidor não
   * responda */

  // recvfrom(...);

  close(fd);
  return;
}
