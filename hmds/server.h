#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <hfs.h>
#include <hdb.h>
#include "../common/parse.h"

#define BACKLOG 25

int bind_socket(struct addrinfo* addr_list);
struct addrinfo* get_server_sockaddr(const char* port);
int wait_for_connection(int sockfd);
void handle_connection(int connectionfd, char* hostname);
