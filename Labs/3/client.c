#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int open_connection(struct addrinfo* addr_list);
struct addrinfo* get_sockaddr(const char* hostname, const char* port);

struct addrinfo* get_sockaddr(const char* hostname, const char* port)
{
	struct addrinfo hints;
	struct addrinfo* results;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; // Return socket addresses for the server's IPv4 addresses
	hints.ai_socktype = SOCK_STREAM; // Return TCP socket addresses
	int retval = getaddrinfo(NULL, port, &hints, &results);
	if (retval)
		errx(EXIT_FAILURE, "%s", gai_strerror(retval));
	return results;

}

int open_connection(struct addrinfo* addr_list)
{
	struct addrinfo* addr;
	int sockfd;
	// Iterate through each addrinfo in the list; stop when we successfully
	// connect to one
	for (addr = addr_list; addr != NULL; addr = addr->ai_next)
	{
		// Open a socket
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		// Try the next address if we couldn't open a socket
		if (sockfd == -1)
			continue;
		// Stop iterating if we're able to connect to the server
		if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) != -1)
			break;
	}
	// Free the memory allocated to the addrinfo list
	freeaddrinfo(addr_list);
	// If addr is NULL, we tried every addrinfo and weren't able to connect to any
		if (addr == NULL)
			err(EXIT_FAILURE, "%s", "Unable to connect");
		else
			return sockfd;
}

int main(int argc, char** argv)
{
	char* msg = "hello world\r\n"; // Message to send
	char buffer[strlen(msg) + 1]; // Buffer to store received message, leaving
								  // space for the NULL terminator
								  // Connect to the server
	struct addrinfo* results = get_sockaddr("localhost", "5000");
	int sockfd = open_connection(results);
	// Send the message
	if (send(sockfd, msg, strlen(msg), 0) == -1)
		err(EXIT_FAILURE, "%s", "Unable to send");
	// Read the echo reply
	int bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read == -1)
		err(EXIT_FAILURE, "%s", "Unable to read");
	// Add the terminating NULL character to the end and print it
	buffer[bytes_read] = '\0';
	printf("Data received: %s", buffer);
	// Close the connection
	close(sockfd);
	exit(EXIT_SUCCESS);

}
