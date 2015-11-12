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

static int verbose_flag = 0;

void parseInput(int argc, char *argv[], char* result[]);
int open_connection(struct addrinfo* addr_list);
struct addrinfo* get_sockaddr(const char* hostname, const char* port);

int main(int argc, char *argv[])
{
	openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	char* options[5] = { 0 };
	parseInput(argc, argv, options);

	//Scan the Hooli root directory (as in assignment 1), generating a list of files/checksums.
	hfs_entry* listRoot = hfs_get_files(options[4]);
	hfs_entry* current = listRoot;

	// Connect to the server
	struct addrinfo* results = get_sockaddr(options[2], options[3]);
	int sockfd = open_connection(results);

	//Issue an AUTH request
	char msg[80];
	strcpy(msg, "AUTH\n");

	strcat(msg, "username:");
	strcat(msg, options[0]);
	strcat(msg, "\n");

	strcat(msg, "password:");
	strcat(msg, options[1]);
	strcat(msg, "\n\n");

	char buffer[80]; // Buffer to store received message, leaving space for the NULL terminator
								
	// Send the message
	if (send(sockfd, &msg, strlen(msg), 0) == -1)
		err(EXIT_FAILURE, "%s", "Unable to send");
	
	// Read the reply
	int bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

	//Check if reply is valid
	if (bytes_read == -1)
		err(EXIT_FAILURE, "%s", "Unable to read");

	//Print the reply
	printf("%s\n", buffer);

	//Send the list of files/checksums in a LIST request to the server. Again, this can (preferably) come from a linked list in memory, or can be read from the temporary file you generated

	// Close the connection
	close(sockfd);

	//Print the list of files requested by the server. If the server requests no files, print an appropriate message.

	exit(EXIT_SUCCESS);

}//End of main method

struct addrinfo* get_sockaddr(const char* hostname, const char* port)
{
	struct addrinfo hints;
	struct addrinfo* results;

	memset(&hints, 0, sizeof(struct addrinfo));

	//Return socket addresses for the server's IPv4 addresses
	hints.ai_family = AF_INET; 
	
	//Return TCP socket addresses
	hints.ai_socktype = SOCK_STREAM;
	
	int retval = getaddrinfo(NULL, port, &hints, &results);
	
	if (retval) errx(EXIT_FAILURE, "%s", gai_strerror(retval));
	
	return results;
}

int open_connection(struct addrinfo* addr_list)
{
	struct addrinfo* addr;
	int sockfd;

	//Iterate through each addrinfo in the list; stop when we successfully
	//connect to one
	for (addr = addr_list; addr != NULL; addr = addr->ai_next)
	{
		//Open a socket
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

		//Try the next address if we couldn't open a socket
		if (sockfd == -1) continue;

		//Stop iterating if we're able to connect to the server
		if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) != -1) break;
	}

	//Free the memory allocated to the addrinfo list
	freeaddrinfo(addr_list);

	//If addr is NULL, we tried every addrinfo and weren't able to connect to any
	if (addr == NULL)
		err(EXIT_FAILURE, "%s", "Unable to connect");
	else
		return sockfd;
		
}

void parseInput(int argc, char *argv[], char* result[])
{
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"verbose", no_argument, &verbose_flag, 1},
			{"server",	required_argument,	0,	's'},
			{"dir",		required_argument,	0,	'd'},
			{"port",	required_argument,	0,	'o'},
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
			result[2] = optarg; 
			break;

		case 'd':
			result[4] = optarg;
			break;

		case 'o':
			result[3] = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			exit(EXIT_FAILURE);
			break;
		}

	}

	if (optind == argc-2)
	{
			result[0] = argv[optind++];
			result[1] = argv[optind++];
	}
	else
	{
		syslog(LOG_ERR, "No Username / Password specified");
		closelog();
		exit(EXIT_FAILURE);
	}

	if (result[2] == 0)
	{
		result[2] = "localhost";
	}
	
	if (result[3] == 0)
	{
		result[3] = "9000";
	}

	if (result[4] == 0)
	{
		result[4] = "~/hooli";
	}

}