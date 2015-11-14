/*
Author: Gurpreet Singh
Description: This is the main file
*/

#include "server.h"

int main(int argc, char *argv[])
{
	openlog("server", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	char* options[2] = { 0 };
	parseInput(argc, argv, options, false);
 
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
}

void handle_connection(int connectionfd, char* hostname)
{
	char buffer[4096];
	int bytes_read;
	char* pch;
	char username[40];
	char password[40];
	char authMsg[80];
	char listMsg[11000];
	listMsg[0] = '\0';
	char listBody[10000];
	listBody[0] = '\0';
	char str[20];

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
      pch = strtok(buffer, "\n");

      while (pch != NULL)
      {
        if (strcmp(pch, "AUTH") == 0)
        {
          pch = strtok(NULL, "\n");
          strcpy(username, pch+9);

          syslog(LOG_INFO, "Username: %s", username);
          
          pch = strtok(NULL, "\n");
          strcpy(password, pch+9);

          //Get the AuthToken
          char* authToken = hdb_authenticate(dbConnection, username, password);
          
          //Check if AUTH is valid from 16-byte alphanumeric authentication token. NULL = Invalid. 
          if (hdb_verify_token(dbConnection, authToken) != NULL)
          {
            syslog(LOG_DEBUG, "Authentication successful");

            strcpy(authMsg, "200 Authentication successful\n");
            strcat(authMsg, "Token:");
            strcat(authMsg, authToken);
            strcat(authMsg, "\n\n");
          }
          else
          {
            syslog(LOG_DEBUG, "Authentication unsuccessful");
            strcpy(authMsg, "401 Unauthorized\n\n");
          }

          // Send the message
          if (send(connectionfd, &authMsg, strlen(authMsg), 0) == -1)
            err(EXIT_FAILURE, "%s", "Unable to send\n");
        }
        else if (strcmp(pch, "LIST") == 0)
        {
          syslog(LOG_INFO, "Receiving file list");

          pch = strtok(NULL, "\n");
          
		  if (hdb_verify_token(dbConnection, pch + 6) != NULL)
		  {
			  pch = strtok(NULL, "\n");

			  char lenBuffer[100];
			  char filename[50];
			  int currLen = 0;
			  int totalLen = 0;

			  strcpy(lenBuffer, pch + 7);
			  lenBuffer[strlen(lenBuffer)] = '\0';

			  totalLen = (int)strtol(lenBuffer, (char **)NULL, 10);

			  while ((currLen <= totalLen))
			  {
				  pch = strtok(NULL, "\n");

				  if (pch == NULL) break;

				  currLen = currLen + strlen(pch) + 1;
				  strcpy(filename, pch);

				  if (hdb_file_exists(dbConnection, username, filename))
				  {
					  pch = strtok(NULL, "\n");

					  syslog(LOG_DEBUG, "* %s, %s", filename, pch);

					  if (strcmp(pch, hdb_file_checksum(dbConnection, username, filename)))
					  {
						  continue;
					  }
					  else
					  {
						  //This file has changed
						  strcat(listBody, filename);
						  strcat(listBody, "\n");
					  }
				  }
				  else
				  {
					  //Skip the checksum token for this file
					  pch = strtok(NULL, "\n");

					  syslog(LOG_DEBUG, "* %s, %s", filename, pch);

					  //This is a new file
					  strcat(listBody, filename);
					  strcat(listBody, "\n");
				  }
			  }
		  }
		  else
		  {
			  strcpy(listMsg, "401 Unauthorized\n\n");
		  }

		  //Remove last newline
		  char *p = listBody;
		  p[strlen(p) - 1] = 0;

		  sprintf(str, "%d", strlen(listBody));

		  if (str > 0)
		  {
			  syslog(LOG_INFO, "Requesting Files:\n%s", listBody);

			  //Files Changed/Requested (302)
			  strcpy(listMsg, "302 Files requested\n");
			  strcat(listMsg, "Length:");
			  strcat(listMsg, str);
			  strcat(listMsg, "\n\n");
			  strcat(listMsg, listBody);
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
