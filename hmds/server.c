/*
Author: Gurpreet Singh
Class: CS3357
Date: Nov, 13 / 2015
Description: This is the main file for the server side
*/

#include "server.h"

int main(int argc, char *argv[])
{
	//Opens a syslog
	openlog("server", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	
	//Declare an array of 5 options to be read in from command line
	char* options[2] = { 0 };

	//Parse the inputs from the command line and populate the options array
	parseInput(argc, argv, options, 1);
 
	// We want to listen on the port specified on the command line
	struct addrinfo* results = get_sockaddr(options[1], true);

	// Create a listening socket
	int sockfd = connection(results, true);

	// Start listening on the socket
	if (listen(sockfd, BACKLOG) == -1)
		err(EXIT_FAILURE, "%s", "Unable to listen on socket");
  
    syslog(LOG_INFO, "Server listening on port %s", options[1]);

	// Wait for a connection
	int connectionfd = wait_for_connection(sockfd);
	handle_connection(connectionfd, options[0]);

	// Close the connection socket
	close(connectionfd);
}//End of main method

void handle_connection(int connectionfd, char* hostname)
{
	char buffer[BUFFERLENGTH];				//A buffer for recieving data
	int bytes_read;						//A count for how many bytes were read
	char* pch;						//A pointer used for tokenizing the recieved message
	char username[CREDENTIALSLENGTH];			//Used to store the user's username
	char password[CREDENTIALSLENGTH];			//Used to store the user's password
	char authMsg[AUTHMESSAGELENGTH];			//Used to store the auth message
	char listMsg[LISTMSGLENGTH];				//Used to store the list message
	listMsg[0] = '\0';					//Nullterminate the list message at the first index (used for strcat) 
	char listBody[LISTBODYLENGTH];				//Used to store the list Body
	listBody[0] = '\0';					//Null terminate the list body so strcat can be used to create string

	syslog(LOG_INFO, "Incoming connection from %s", hostname);

	do
	{
		// Read up to 4095 bytes from the client
		bytes_read = recv(connectionfd, buffer, sizeof(buffer) - 1, 0);

		//Create a DB Connection
		hdb_connection* dbConnection = hdb_connect(hostname);

		// If the data was read successfully
		if (bytes_read > 0)
		{
			//Read in the first token of the string
			pch = strtok(buffer, "\n");

			//While there are still tokens remaining in the string
			while (pch != NULL)
			{
				//Check if the first token (request type) was AUTH
				if (strcmp(pch, "AUTH") == 0)
				{
					//If it was, read in the next line (username)
					pch = strtok(NULL, "\n");
					strcpy(username, pch + strlen("Username:"));

					syslog(LOG_INFO, "Username: %s", username);

					//Read in the next line, password and store
					pch = strtok(NULL, "\n");
					strcpy(password, pch + strlen("Password:"));

					//Get the AuthToken
					char* authToken = hdb_authenticate(dbConnection, username, password);

					//Check if AUTH is valid from 16-byte alphanumeric authentication token. NULL = Invalid. 
					if (hdb_verify_token(dbConnection, authToken) != NULL)
					{
						syslog(LOG_DEBUG, "Authentication successful");

						//Build the AUTH successful response 
						sprintf(authMsg, "200 Authentication successful\nToken:%s\n\n", authToken);
					}
					else
					{
						//Prepare unsuccessful responnse 
						syslog(LOG_DEBUG, "Authentication unsuccessful");
						strcpy(authMsg, "401 Unauthorized\n\n");
					}

					// Send the message
					if (send(connectionfd, &authMsg, strlen(authMsg), 0) == -1)
						err(EXIT_FAILURE, "%s", "Unable to send\n");
				}
				//If the first token indicated a LIST request
				else if (strcmp(pch, "LIST") == 0)
				{
					syslog(LOG_INFO, "Receiving file list");

					//read the next line (the token)
					pch = strtok(NULL, "\n");

					if (hdb_verify_token(dbConnection, pch + strlen("Token:")) != NULL)
					{
						pch = strtok(NULL, "\n");

						char lenBuffer[100];			//A place to store the length of the file line
						char filename[50];				//A place to store the filename
						int currLen = 0;				//Store the current amount of letters read in
						int totalLen = 0;				//Store the total amount of letters expected

						//Extract the length key/value from the message
						strcpy(lenBuffer, pch + strlen("Length:"));
						lenBuffer[strlen(lenBuffer)] = '\0';

						//Parse length read in into an int and store as expected total
						totalLen = (int)strtol(lenBuffer, (char **)NULL, 10);

						//Keep reading in data until expected total characters are reached
						while ((currLen <= totalLen))
						{
							//Read in a line
							pch = strtok(NULL, "\n");

							//If its the last line, break out
							if (pch == NULL) break;

							//Increment the current character counter (+1 at the end to account for \n character)
							currLen = currLen + strlen(pch) + 1;

							//Copy the current token in as the filename for the current file
							strcpy(filename, pch);

							//Check if the file already exists
							if (hdb_file_exists(dbConnection, username, filename))
							{
								//Read in the next line (checksum for file)
								pch = strtok(NULL, "\n");

								syslog(LOG_DEBUG, "* %s, %s", filename, pch);

								//Check if checksum is equal to the file_checksum stored
								if (strcmp(pch, hdb_file_checksum(dbConnection, username, filename)))
								{
									//If so do nothing and move to next file
									continue;
								}
								else
								{
									//This file has changed, store it's name in files to be sent back in response
									sprintf(listBody, "%s\n", filename);
								}
							}
							else
							{
								//Skip the checksum token for this file
								pch = strtok(NULL, "\n");

								syslog(LOG_DEBUG, "* %s, %s", filename, pch);

								//This is a new file, store it's name in files to be sent back in response
								sprintf(listBody, "%s\n", filename);
							}
						}
					}
					else
					{
						//If the user is unauthorized store that to send back
						strcpy(listMsg, "401 Unauthorized\n\n");
					}

					//Remove last newline because now allowed in specs
					char *p = listBody;
					p[strlen(p) - 1] = 0;

					char str[20];				//Temporary string used to store 

					//Copy listBody's length into temp variable
					sprintf(str, "%d", strlen(listBody));

					//If the length is greater than zero generate a 302 request
					if (str > 0)
					{
						syslog(LOG_INFO, "Requesting Files:\n%s", listBody);

						//Files Changed/Requested (302)
						sprintf(listMsg, "302 Files requested\nLength:%s\n\n%s", str, listBody);
					}
					else
					{
						syslog(LOG_INFO, "Requesting No Files");

						//No Files Changed/Requested (204)
						strcpy(listMsg, "204 No files requested\n\n");
					}

					// Send the message
					if (send(connectionfd, &listMsg, strlen(listMsg), 0) == -1)
						err(EXIT_FAILURE, "%s", "Unable to send\n");

				}
				pch = strtok(NULL, "\n");
			}
		}
	} while (bytes_read > 0);

	syslog(LOG_INFO, "Connection terminated");

	// Close the connection
	close(connectionfd);
}

// wait_for_connection(int sockfd)
// This method waits for a connection on a specific socket and then connects to it when possible
//
// sockfd: The socket that is being listened too  
int wait_for_connection(int sockfd)
{
	struct sockaddr_in client_addr;						// Remote IP that is connecting to us
	unsigned int addr_len = sizeof(struct sockaddr_in); 			// Length of the remote IP structure
	char ip_address[INET_ADDRSTRLEN];					// Buffer to store human-friendly IP address
	int connectionfd;							// Socket file descriptor for the new connection
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
