#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef UDP_SOCKETS_H
#define UDP_SOCKETS_H

#define UDP_MSS 65535

typedef struct
{
  int length;
  uint8_t buffer[UDP_MSS];
} message;

typedef struct
{
  struct sockaddr_in addr;
  socklen_t addr_len;
  char friendly_ip[INET_ADDRSTRLEN];
} host;
                                                                                    
struct addrinfo* get_udp_sockaddr(const char* node, const char* port, int flags);
message* create_message();
message* receive_message(int sockfd, host* source);
int send_message(int sockfd, message* msg, host* dest);

#endif

