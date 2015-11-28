#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udp_client.h"
                                                                        
int create_client_socket(char* hostname, char* port, host* server)
{
  int sockfd;
  struct addrinfo* addr;
  struct addrinfo* results = get_udp_sockaddr(hostname, port, 0);                 

  // Iterate through each addrinfo in the list;
  // stop when we successfully create a socket
  for (addr = results; addr != NULL; addr = addr->ai_next)                       
  {
    // Open a socket
    sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);      

    // Try the next address if we couldn't open a socket
    if (sockfd == -1)
      continue;

    // Copy server address and length to the out parameter 'server'
    memcpy(&server->addr, addr->ai_addr, addr->ai_addrlen);
    memcpy(&server->addr_len, &addr->ai_addrlen, sizeof(addr->ai_addrlen));

    // We've successfully created a socket; stop iterating
    break;
  }

  // Free the memory allocated to the addrinfo list
  freeaddrinfo(results);

  // If we tried every addrinfo and failed to create a socket
  if (addr == NULL)
  {
    perror("Unable to create socket");
    exit(EXIT_FAILURE);
  }
  else
  {
    // Otherwise, return the socket descriptor
    return sockfd;                                                               
  }
}
