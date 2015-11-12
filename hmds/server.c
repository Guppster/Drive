/*
Author: Gurpreet Singh
Description: This is the main file
*/

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <hfs.h>
#include <hdb.h>

#define BACKLOG 25

static int verbose_flag = 0;

int bind_socket(struct addrinfo* addr_list);
struct addrinfo* get_server_sockaddr(const char* port);
int wait_for_connection(int sockfd);
void handle_connection(int connectionfd, char* hostname);
void parseInput(int argc, char *argv[], char* result[]);

int main(int argc, char *argv[])
{
	openlog("server", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	char* options[2] = { 0 };
	parseInput(argc, argv, options);

	// We want to listen on the port specified on the command line
	struct addrinfo* results = get_server_sockaddr(options[1]);

	// Create a listening socket
	int sockfd = bind_socket(results);

	// Start listening on the socket
	if (listen(sockfd, BACKLOG) == -1)
		err(EXIT_FAILURE, "%s", "Unable to listen on socket");

	// Wait for a connectioni
	int connectionfd = wait_for_connection(sockfd);
	handle_connection(connectionfd, options[0]);

	// Close the connection socket
	close(connectionfd);
}

void handle_connection(int connectionfd, char* hostname)
{
	char buffer[4096];
	int bytes_read;
	char* pch;
	char username[40];
	char password[40];
	char msg[80];

	do
	{
		// Read up to 4095 bytes from the client
		bytes_read = recv(connectionfd, buffer, sizeof(buffer) - 1, 0);

		// If the data was read successfully
		if (bytes_read > 0)
		{
			pch = strtok(buffer, "\n");

			while (pch != NULL)
			{
				if (strcmp(pch, "AUTH") == 0)
				{
					pch = strtok(NULL, "\n");
					strcpy(username, pch+9);

					printf("Authenticating User: %s \n", username);

					pch = strtok(NULL, "\n");
					strcpy(password, pch+9);

					//Create a DB Connection
					hdb_connection* dbConnection = hdb_connect(hostname);

					//Check if DB Connection is valid.
		
					//Get the AuthToken
					char* authToken = hdb_authenticate(dbConnection, username, password);
					
					//Check if AUTH is valid from 16-byte alphanumeric authentication token. NULL = Invalid. 
					if (hdb_verify_token(dbConnection, authToken) != NULL)
					{
						strcpy(msg, "200 Authentication successful\n");
						strcat(msg, "Token:");
						strcat(msg, authToken);
						strcat(msg, "\n\n");
					}
					else
					{
						strcpy(msg, "401 Unauthorize\n\n");
					}

					// Send the message
					if (send(connectionfd, &msg, strlen(msg), 0) == -1)
						err(EXIT_FAILURE, "%s", "Unable to send\n");

				}
				else if (strcmp(pch, "LIST") == 0)
				{

				}

				pch = strtok(NULL, "\n");

			}

		}
	} while (bytes_read > 0);

	// Close the connection
	close(connectionfd);
}

struct addrinfo* get_server_sockaddr(const char* port)
{
	struct addrinfo hints;
	struct addrinfo* results;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET; // Return socket addresses for our local IPv4 addresses
	hints.ai_socktype = SOCK_STREAM; // Return TCP socket addresses
	hints.ai_flags = AI_PASSIVE; // Socket addresses should be for listening sockets

	int retval = getaddrinfo(NULL, port, &hints, &results);

	if (retval)
		errx(EXIT_FAILURE, "%s", gai_strerror(retval));
	return results;
}

int bind_socket(struct addrinfo* addr_list)
{
	struct addrinfo* addr;
	int sockfd;
	char yes = '1';
	// Iterate through each addrinfo in the list; stop when we successfully bind
	// to one
	for (addr = addr_list; addr != NULL; addr = addr->ai_next)
	{

		// Open a socket
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

		// Try the next address if we couldn't open a socket
		if (sockfd == -1)
			continue;

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

	// Free the memory allocated to the address list
	freeaddrinfo(addr_list);

	// If addr is NULL, we tried every address and weren't able to bind to any
	if (addr == NULL)
	{
		err(EXIT_FAILURE, "%s", "Unable to bind");
	}
	else
	{
		// Otherwise, return the socket descriptor
		return sockfd;
	}
}

int wait_for_connection(int sockfd)
{
	struct sockaddr_in client_addr; // Remote IP that is connecting to us
	unsigned int addr_len = sizeof(struct sockaddr_in); // Length of the remote IP structure
	char ip_address[INET_ADDRSTRLEN]; // Buffer to store human-friendly IP address
	int connectionfd; // Socket file descriptor for the new connection
					  // Wait for a new connection

	connectionfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);

	// Make sure the connection was established successfully
	if (connectionfd == -1)
		err(EXIT_FAILURE, "%s", "Unable to accept connection");

	// Convert the connecting IP to a human-friendly form and print it
	inet_ntop(client_addr.sin_family, &client_addr.sin_addr, ip_address, sizeof(ip_address));

	printf("Connection accepted from %s\n", ip_address);

	// Return the socket file descriptor for the new connection
	return connectionfd;
}

void parseInput(int argc, char *argv[], char* result[])
{
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{ "verbose", no_argument, &verbose_flag, 1 },
			{ "server",	required_argument,	0,	's' },
			{ "port",	required_argument,	0,	'o' },
			{ 0, 0, 0, 0 }
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "vs:d:o:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 'v':
			verbose_flag = 1;
			break;

		case 's':
			result[0] = optarg;
			break;

		case 'o':
			result[1] = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			exit(EXIT_FAILURE);
			break;
		}

	}

	if (result[0] == 0)
	{
		result[0] = "localhost";
	}

	if (result[1] == 0)
	{
		result[1] = "9000";
	}
}