#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udp_sockets.h"
                                                                                    
struct addrinfo* get_udp_sockaddr(const char* node, const char* port, int flags)
{
  struct addrinfo hints;
  struct addrinfo* results;
  int retval;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;      // Return socket addresses for our local IPv4 addresses
  hints.ai_socktype = SOCK_DGRAM; // Return UDP socket addresses                                   
  hints.ai_flags = flags;         // Socket addresses should be listening sockets                     
                         
  retval = getaddrinfo(node, port, &hints, &results);

  if (retval != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
    exit(EXIT_FAILURE);
  }

  return results;
}

message* create_message()
{
  return (message*)malloc(sizeof(message));                                                   
}
                                                               
message* receive_message(int sockfd, host* source)
{
  message* msg = create_message();                                                                          

  // Length of the remote IP structure
  source->addr_len = sizeof(source->addr);

  // Read message, storing its contents in msg->buffer, and
  // the source address in source->addr
  msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,                  
                         (struct sockaddr*)&source->addr,                           
                         &source->addr_len);

  // If a message was read
  if (msg->length > 0)
  {
    // Convert the source address to a human-readable form,
    // storing it in source->friendly_ip
    inet_ntop(source->addr.sin_family, &source->addr.sin_addr,                  
              source->friendly_ip, sizeof(source->friendly_ip));                  

    // Return the message received
    return msg;                                                               
  }
  else
  {
    // Otherwise, free the allocated memory and return NULL
    free(msg);                                                               
    return NULL;                                                               
  }
}

int send_message(int sockfd, message* msg, host* dest)
{
  return sendto(sockfd, msg->buffer, msg->length, 0,
                (struct sockaddr*)&dest->addr, dest->addr_len);
}

