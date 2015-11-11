#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SOCK_TYPE(s) (s == SOCK_STREAM ? "Stream" : s == SOCK_DGRAM ? "Datagram" : \
 s == SOCK_RAW ? "Raw" : "Other")

int main(int argc, char** argv)
{
	struct addrinfo hints; // Hints passed to getaddrinfo

	struct addrinfo* results; // Linked list of results populated by getaddrinfo

	struct addrinfo* res; // Pointer to a result in the linked list

	// Buffer to store human-readable IP address
	char ip_address[INET_ADDRSTRLEN];

	// Initialize hints and request IPv4 addresses
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	// Attempt to resolve the IP address
	int retval = getaddrinfo(argv[1], NULL, &hints, &results);

	// On failure, convert return value to human-readable error, print, and exit
	if (retval)
		errx(EXIT_FAILURE, "%s", gai_strerror(retval));


	// Iterate through each result in the results linked list
	for (res = results; res != NULL; res = res->ai_next)
	{
		// Cast the result's address to a Internet socket address
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;

		// Convert it from its packed, binary form to a human readable string
		inet_ntop(res->ai_family, &ipv4->sin_addr, ip_address, sizeof(ip_address));

		// Display the IP address, socket type, and protocol
		printf("%-15s %-10s %s\n", ip_address, SOCK_TYPE(res->ai_socktype),
			getprotobynumber(res->ai_protocol)->p_name);
	}

	// Free the memory allocated to the linked list
	freeaddrinfo(results);
	exit(EXIT_SUCCESS);
}