#include "join_module.h"
#include "../../error_handling/error_checking.h"
#include "../../error_handling/error_messages.h"
#include "../../essentials/host_handling.h"
#include "../../fildes_handling/core/TCP.h"
#include "../../fildes_handling/core/UDP.h"
#include "../custom_protocols_interface/delete_node_module.h"
#include "leave_module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPORT 65535
#define MAXNODES 99
#define NETSIZE 3
#define IDSIZE 2
#define SIZE 64

/**
 * @brief Find a new external node for the host, skipping nodes in the blacklist
 * @note The blacklist is reset if the host's network has changed since the last call or if no
 * suitable extern node has been found
 *
 * @param host: the host structure containing network information and the ID to blacklist
 * @param blacklist_ID: the ID of the node to add to the blacklist
 * @return a dynamically allocated string containing the found node information or NULL if no
 * suitable node is found
 */
static char *find_new_extern(host *host, int blacklist_ID) {
  static int last_net = -1;
  static int blacklist[100] = {0};
  int current_net = atoi(host->net);

  // Reset the blacklist if the network has changed
  if (last_net != current_net) {
    memset(blacklist, 0, sizeof(blacklist));
    last_net = current_net;
  }

  // Add itself to the blacklist
  blacklist[atoi(host->ID)] = 1;

  // Add the specified node ID to the blacklist
  blacklist[blacklist_ID] = 1;

  // Allocate memory for the message to send
  char *msg_to_send = calloc(SIZE, sizeof(char));
  if (msg_to_send == NULL) {
    die_with_system_error(host, "calloc() failed");
  }

  // Prepare the message to request the list of nodes
  sprintf(msg_to_send, "NODES %s", host->net);
  char *received_nodeslist = send_and_receive_msg_UDP(host, msg_to_send);
  if (received_nodeslist == NULL) {
    free(msg_to_send);
    return NULL;
  }

  const char *p = received_nodeslist;
  char ID[8], IP[16], TCP[8];

  // Iterate through the list of nodes
  while (p) {
    p = strchr(p, '\n');
    if (p) {
      p++; // Move to the character after the newline
      if (sscanf(p, "%2s %15s %5s", ID, IP, TCP) == 3) {
        if (blacklist[atoi(ID)]) {
          continue; // Skip the blacklisted node
        }

        // Prepare the message to send with the found node information
        sprintf(msg_to_send, "djoin %s %s %s %s %s\n", host->net, host->ID, ID, IP, TCP);
        free(received_nodeslist);

        return msg_to_send;
      }
    }
  }

  // No suitable node found
  free(msg_to_send);
  free(received_nodeslist);

  // Reset blacklist for future attempts
  memset(blacklist, 0, sizeof(blacklist));
  return NULL;
}

/**
 * @brief Checks and updates an ID to ensure its uniqueness in the network.
 *
 * This function verifies if the given ID is unique within the specified node list.
 * If the ID is not unique, the function generates new IDs iteratively and checks
 * their uniqueness until a unique ID is found or the maximum number of collisions is
 * reached.
 *
 * @param host: pointer to the host structure
 * @param node_list: null-terminated string containing the list of nodes to check
 * @param ID: pointer to a string containing the ID to check and/or modify
 */
static void check_uniqueness_of_ID(host *host, char *node_list, char (*ID)[SIZE]) {
  int new_id = 0, PORT = 0;
  char pattern[SIZE] = {'\0'}, IP[16] = {'\0'};

  // Build the pattern
  snprintf(pattern, SIZE, "\n%s", (*ID));

  // Check if the pattern is present in the node list
  char *str = strstr(node_list, pattern);
  if (str == NULL) {
    return;
  }

  // Extract IP and PORT from the matched pattern
  if (sscanf(str, "%*s %s %d", IP, &PORT) != 2) {
    system_error("sscanf() failed");
    return;
  }

  // Check if the IP and PORT match the host's IP and TCP port
  if (strcmp(IP, host->uip->IP) == 0 && PORT == host->uip->TCP) {
    // Do nothing, server will reply with OKREG
    return;
  }

  // Generate a new unique ID
  while (new_id <= MAXNODES) {
    // Generate a new ID
    snprintf((*ID), SIZE, "%02d", new_id);

    // Update the pattern with the new ID
    snprintf(pattern, SIZE, "\n%s", (*ID));

    // Check if the new pattern is unique
    if (strstr(node_list, pattern) == NULL) {
      printf(YELLOW "[NOTICE]" RESET " ID already in use. Using ID: %s\n", (*ID));
      break;
    }

    new_id++;
  }

  // Make sure to update the ID in case it exceeds the limit
  snprintf((*ID), SIZE, "%02d", new_id);
}

/**
 * @brief Fetches a random external node from a node list string.
 * @note Assumes the first token in the list is the header: NODESLIST XXX.
 *
 * @param node_list: string containing the node list, separated by newline characters.
 *
 * @return pointer to a string containing the needed information of the randomly
 * selected external node. Returns NULL if no suitable external node is found.
 */
static char *fetch_extern_from_nodelist(char *node_list) {
  char *token = strtok(node_list, "\n");
  char *array[MAXNODES] = {NULL};
  int node_count = 0;

  // Tokenize the node list and store each node in the array
  while ((token = strtok(NULL, "\n")) != NULL) {
    array[node_count++] = token;
  }

  // Check if there is at least one external node in the list
  if (node_count == 0) {
    return NULL; // The first node in the server's list
  }

  // Return a random external node from the array
  return array[rand() % node_count];
}

/**
 * @brief Assigns the ID and network values to the host and initializes the
 * forwarding table.
 *
 * This function allocates memory for the host ID and network, copies the values from
 * the given parameters, and initializes the forwarding table with the host ID.
 *
 * @param host: The host structure containing the ID, network, and forwarding table.
 * @param ID: The host's ID to be assigned.
 * @param net: The network value to be assigned to the host.
 */
static void assign_host_ID_and_network(host *host, const char *ID, const char *net) {
  if (host->net != NULL) {
    return;
  }

  // Allocate memory for the host ID
  host->ID = (char *)malloc((IDSIZE + 1) * sizeof(char));
  if (host->ID == NULL) {
    die_with_system_error(host, "malloc() failed");
  }

  // Allocate memory for the host network
  host->net = (char *)malloc((NETSIZE + 1) * sizeof(char));
  if (host->net == NULL) {
    die_with_system_error(host, "malloc() failed");
  }

  // Copy the ID value to the host
  if (snprintf(host->ID, IDSIZE + 1, "%2s", ID) < IDSIZE) {
    die_with_system_error(host, "snprintf() failed");
  }

  // Copy the network value to the host
  if (snprintf(host->net, NETSIZE + 1, "%3s", net) < NETSIZE) {
    die_with_system_error(host, "snprintf() failed");
  }

  // Initialize the forwarding table with the host ID
  insert_in_forwarding_table(host, atoi(ID), atoi(ID));
}

/**
 * @brief Debug join a network
 *
 * This function allows a host to join a network for debugging purposes. It processes
 * the input parameters and attempts to join the network by connecting to a node. If
 * successful, it establishes connections.
 *
 * @param buffer: buffer containing the command and parameters for the join
 * @param host: pointer to the host structure representing the host joining the
 * network
 * @param flag: flag indicating if the join call was made directly by the user or
 * from a function call
 *
 * @return 1 if the operation was successful, 0 otherwise
 */
int djoin_network(char *buffer, host *host, user_command flag) {
  if (host->net != NULL) {
    printf(YELLOW "[NOTICE]" RESET " Already registered in network %s.\n", host->net);
    return 0;
  }

  char net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};
  char node_ID[SIZE] = {'\0'}, node_IP[SIZE] = {'\0'}, node_TCP[SIZE] = {'\0'};

  if (sscanf(buffer, "djoin %s %s %s %s %s\n", net, ID, node_ID, node_IP, node_TCP) != 5) {
    user_input_error("Invalid `djoin` call", buffer,
                     "Type 'help' or '?' for more information.\n");
    return 0;
  }

  // Verify arguments for direct user call to djoin
  if (flag == DJOIN) {
    if (!check_net_and_id(net, ID) || !check_node_parameters(node_ID, node_IP, node_TCP)) {
      return 0;
    }
  }

  // Single node in the network
  if (strcmp(ID, node_ID) == 0) {
    assign_host_ID_and_network(host, ID, net);
    printf(YELLOW "[NOTICE]" RESET " Successfully connected node %s in net %s\n", ID,
           host->net);
    return 1;
  }

  insert_node(node_ID, -1, node_IP, atoi(node_TCP), host);
  assign_host_ID_and_network(host, ID, net);

  // Message exchange between the host and the external node
  if (!get_a_new_backup(host)) {
    free_node(host->ext);
    host->node_list = host->ext = NULL;

    if (flag == DJOIN) {
      fprintf(stderr, "Unable to connect to the requested node %s!\n", node_ID);
      leave_network(host, DJOIN);
      return 0;
    }

    char *new_djoin_msg = find_new_extern(host, atoi(node_ID));
    if (new_djoin_msg == NULL) {
      fprintf(stderr,
              YELLOW "[NOTICE] " RESET " Unable to connect to any node in the network (%s)!\n",
              net);
      return 1;
    }

    int r = djoin_network(new_djoin_msg, host, JOIN);
    free(new_djoin_msg);
    return r;
  }

  insert_in_forwarding_table(host, atoi(host->ext->ID), atoi(host->ext->ID));
  printf(YELLOW "[NOTICE]" RESET " Successfully connected node %s in net %s\n", ID, host->net);
  return 1;
}

/**
 * @brief Join a network given its name and a unique ID
 *
 * This function attempts to join a network based on the provided name and ID.
 * It performs necessary checks and, if successful, connects the host to the network.
 *
 * @param buffer: input buffer with the user command and arguments
 * @param host: pointer to the `host` struct
 *
 * @return 1 if the host successfully joined the network, 0 otherwise
 */
int join_network(char *buffer, host *host) {
  char net[SIZE] = {'\0'}, ID[SIZE] = {'\0'};

  if (sscanf(buffer, "join %s %s\n", net, ID) != 2) {
    user_input_error("Invalid `join` call", buffer,
                     "Type 'help' or '?' for more information.\n");
    return 0;
  }

  if (!check_net_and_id(net, ID)) {
    return 0;
  }

  // Check if the host is already in a network
  if (host->net != NULL) {
    printf(YELLOW "[NOTICE]" RESET " Already registered in network %s\n", host->net);
    return 0;
  }

  char msg_to_send[SIZE << 2] = {'\0'};
  sprintf(msg_to_send, "NODES %s", net);
  char *received_nodeslist = send_and_receive_msg_UDP(host, msg_to_send);
  if (received_nodeslist == NULL) {
    return 0;
  }

  check_uniqueness_of_ID(host, received_nodeslist, &ID);
  if (atoi(ID) > MAXNODES) {
    fprintf(stderr, YELLOW "[NOTICE] " RESET);
    fprintf(stderr, "Network is full! Couldn't register in the network (%s).\n", net);
    free(received_nodeslist);
    return 0;
  }

  memset(msg_to_send, 0, sizeof(msg_to_send));
  sprintf(msg_to_send, "REG %s %s %s %d", net, ID, host->uip->IP, host->uip->TCP);
  char *received_reg_msg = send_and_receive_msg_UDP(host, msg_to_send);
  if (received_reg_msg == NULL) {
    free(received_nodeslist);
    return 0;
  }

  if (strcmp(received_reg_msg, "OKREG") == 0) {
    memset(msg_to_send, 0, sizeof(msg_to_send));
    char *ext_node = fetch_extern_from_nodelist(received_nodeslist);
    if (ext_node != NULL) {
      sprintf(msg_to_send, "djoin %s %s %s", net, ID, ext_node);
      djoin_network(msg_to_send, host, JOIN); // connects to the ext node in the network
    } else {
      assign_host_ID_and_network(host, ID, net);
      printf(YELLOW "[NOTICE]" RESET " Successfully connected node %s in net %s\n", ID,
             host->net);
    }
  } else {
    fprintf(stderr, YELLOW "[NOTICE] " RESET);
    fprintf(stderr,
            "Unexpected server response: " RED " %s " RESET
            " Couldn't register in the network (%s).\n",
            received_reg_msg, net);
    free(received_nodeslist), free(received_reg_msg);
    return 0;
  }

  free(received_nodeslist), free(received_reg_msg);
  return 1;
}
