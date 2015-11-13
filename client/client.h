
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <hfs.h>
#include "../common/parse.h"

int open_connection(struct addrinfo* addr_list);
struct addrinfo* get_sockaddr(const char* hostname, const char* port);
