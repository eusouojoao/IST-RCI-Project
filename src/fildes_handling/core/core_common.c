#include "../../error_handling/error_messages.h"
#include "core_common.h"

#include <string.h>
#include <sys/socket.h>

/**
 * @brief Creates a new UDP socket.
 *
 * This helper function creates a new UDP socket by calling the socket() function
 * with the appropriate parameters for a UDP socket. It also handles error checking
 * in case the socket creation fails.
 *
 * @return The file descriptor of the created UDP socket, or -1 on error.
 */
int create_socket(socket_type type) {
  int std_type = type == UDP ? SOCK_DGRAM : SOCK_STREAM;

  int fd = socket(AF_INET, std_type, 0);
  if (fd == -1) {
    system_error("socket() failed");
  }
  return fd;
}

/**
 * @brief Initializes a sockaddr_in structure with the given IP and port.
 *
 * This helper function sets up a sockaddr_in structure by filling in its
 * fields with the provided IP address and port. It also handles the
 * conversion of the IP address from a string to the binary form.
 *
 * @param addr: pointer to the sockaddr_in structure to be initialized
 * @param ip: IP address as a string
 * @param port: port number
 *
 * @return 0 on success, -1 on error.
 */
int setup_sockaddr_in(struct sockaddr_in *addr, const char *ip, int port) {
  memset(addr, 0, sizeof(*addr));
  if (inet_pton(AF_INET, ip, &(addr->sin_addr)) != 1) {
    system_error("inet_pton() failed");
    return -1;
  }

  // Set the address family and port number
  addr->sin_family = AF_INET;
  addr->sin_port = htons((in_port_t)port);
  return 0;
}
