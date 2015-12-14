#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <netdb.h>
#include <hdb.h>
#include <stdio.h>
#define BACKLOG 25

int connection(struct addrinfo* addr_list, bool open)
{
	struct addrinfo* addr;
	int sockfd;
	char yes = '1';

	//Iterate through each addrinfo in the list; stop when we successfully
	//connect to one
	for (addr = addr_list; addr != NULL; addr = addr->ai_next)
	{
		//Open a socket
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

		//Try the next address if we couldn't open a socket
		if (sockfd == -1) continue;

		if (open)
		{
			// Allow the port to be re-used if currently in the TIME_WAIT state
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				err(EXIT_FAILURE, "%s", "Unable to set socket option");

			// Try to bind the socket to the address/port
			if (bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1)
			{
				// If binding fails, close the socket, and move on to the next address
				close(sockfd);
				continue;
			}
			else
			{
				// Otherwise, we've bound the address to the socket, so stop processing
				break;
			}
		}
		else
		{
			//Stop iterating if we're able to connect to the server
			if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) != -1) break;
		}
	}

	//Free the memory allocated to the addrinfo list
	freeaddrinfo(addr_list);

	//If addr is NULL, we tried every addrinfo and weren't able to connect to any
	if (addr == NULL)
		err(EXIT_FAILURE, "%s", "Unable to connect");
	else
		return sockfd;
}

struct addrinfo* get_sockaddr(const char* port, bool isServer)
{
	struct addrinfo hints;
	struct addrinfo* results;

	memset(&hints, 0, sizeof(struct addrinfo));

	//Return socket addresses for the server's/local IPv4 addresses
	hints.ai_family = AF_INET;

	if (isServer)
	{
		hints.ai_socktype = SOCK_STREAM; // Return TCP socket addresses
		hints.ai_flags = AI_PASSIVE; // Socket addresses should be for listening sockets
	}

	//Return TCP socket addresses
	hints.ai_socktype = SOCK_STREAM;

	int retval = getaddrinfo(NULL, port, &hints, &results);

	if (retval) errx(EXIT_FAILURE, "%s", gai_strerror(retval));

	return results;
}
