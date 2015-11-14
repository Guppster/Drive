/*
Author: Gurpreet Singh
Description: This is the main file
*/

#include "client.h"

int main(int argc, char *argv[])
{
	openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	char* options[5] = { 0 };
	char* token;
	char body[10000];	
	body[0] = '\0';
	char str[1000] = "";

	parseInput(argc, argv, options, true);

	syslog(LOG_INFO, "Scanning Hooli directory: %s", options[4]);

	//Scan the Hooli root directory, generating a list of files/checksums.
	hfs_entry* listRoot = hfs_get_files(options[4]);

  if(listRoot == NULL)
  {
	  syslog(LOG_INFO, "Unable to find any files in directory tree");
    exit(EXIT_FAILURE);
  }
  
  hfs_entry* current = listRoot;

	do
	{
		strcat(body, current->rel_path);
		strcat(body, "\n");

		sprintf(str, "%x", current->crc32);

		syslog(LOG_DEBUG, "* %s (%s)", current->rel_path, str);

		strcat(body, str);
		strcat(body, "\n");

		current = current->next;
	} while (current != NULL);

	//Remove last newline
	char *p = body;
	p[strlen(p) - 1] = 0;

    //Connect to the server
	syslog(LOG_INFO, "Connecting to server");
	struct addrinfo* results = get_sockaddr(options[3], false);
	int sockfd = connection(results, false);

	//Issue an AUTH request
	char authMsg[strlen("AUTH\n") + strlen("username:") + sizeof(options[0]) + strlen("\n") + strlen("password:") + sizeof(options[1]) + strlen("\n\n")];
	strcpy(authMsg, "AUTH\n");

	strcat(authMsg, "username:");
	strcat(authMsg, options[0]);
	strcat(authMsg, "\n");

	strcat(authMsg, "password:");
	strcat(authMsg, options[1]);
	strcat(authMsg, "\n\n");

	char bufferAuthRequest[500]; // Buffer to store received message, leaving space for the NULL terminator
	
	syslog(LOG_DEBUG, "Sending Credentials");

	sendToServer(sockfd, authMsg, bufferAuthRequest);
	
	if (strncmp(bufferAuthRequest, "200", 3) != 0)
	{
		syslog(LOG_INFO, "Connecting Failed");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_INFO, "Authentication successful");

	token = strstr(bufferAuthRequest, "Token:");
	token[strlen(token) - 1] = '\0';

	//Obtain the length of the body and store it in str (temp variable)
	sprintf(str, "%d", strlen(body));

	//Send the list of files/checksums in a LIST request to the server. 
	//Create a LIST request
	char msgList[strlen("LIST\n") + strlen("Token:") + sizeof(token) + strlen("\n") + strlen("Length:") + sizeof(str) + strlen("\n\n") + sizeof(body)];

	strcpy(msgList, "LIST\n");

	strcat(msgList, token);

	strcat(msgList, "Length:");

	strcat(msgList, str);
	strcat(msgList, "\n\n");
	
	strcat(msgList, body);

	// Buffer to store received message, leaving space for the NULL terminator
	char bufferListRequest[10000]; 

    syslog(LOG_INFO, "Uploading file list");

	sendToServer(sockfd, msgList, bufferListRequest);;

    syslog(LOG_INFO, "Server requested the following files:\n%s", strstr(bufferListRequest, "\n\n") + 2);

	// Close the connection
	close(sockfd);
	
	//Print the list of files requested by the server. If the server requests no files, print an appropriate message.
	exit(EXIT_SUCCESS);

}//End of main method

void sendToServer(int sockfd, char* msg, char* buffer)
{
	// Send the message
	if (send(sockfd, msg, strlen(msg), 0) == -1)
		err(EXIT_FAILURE, "%s", "Unable to send");

	// Read the reply
	int bytes_read = recv(sockfd, buffer, 10000, 0);

	//Check if reply is valid
	if (bytes_read == -1)
		err(EXIT_FAILURE, "%s", "Unable to read");
}//End of sendToServer method