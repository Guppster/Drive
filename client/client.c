/*
Author: Gurpreet Singh
Class: CS3357
Date: Nov, 13 / 2015
Description: This is the main file for the client side
*/

#include "client.h"

int main(int argc, char *argv[])
{
	//Open a syslog for logging purposes
	openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);

	char* options[7] = { 0 };			//Declare an array of 7 options to be read in from command line
	char token[20];						//Declare a char array for the token
	char body[LISTBODYLENGTH];			//Declare a char array to hold the body of the list message
	body[0] = '\0';						//Null terminate the char array at the first index (to allow strcat to work)
	char str[TEMPSTRLENGTH] = "";		//Declare a temporary string called STR to store checksum/length of body
	//int nextSeq = 0;

	//Parse the inputs from the command line and populate the options array
	parseInput(argc, argv, options, 0);

	syslog(LOG_INFO, "Scanning Hooli directory: %s", options[4]);

	//Scan the Hooli root directory, generating a list of files/checksums.
	hfs_entry* listRoot = hfs_get_files(options[4]);

	//Check if the scanned directory was empty, if so tell the user and exit
	if (listRoot == NULL)
	{
		syslog(LOG_INFO, "Unable to find any files in directory tree");
		exit(EXIT_FAILURE);
	}

	//Set a pointer to the root/head of the linked list
	hfs_entry* current = listRoot;

	//for each entry in listRoot, add too body
	do
	{
		//Add the relative location of file to the body
		strcat(body, current->rel_path);
		strcat(body, "\n");

		//Convert the string to Uppercase HEX
		sprintf(str, "%X", current->crc32);

		syslog(LOG_DEBUG, "* %s (%s)", current->rel_path, str);

		//Add the checksum to body
		strcat(body, str);
		strcat(body, "\n");

		//Move on to the next file
		current = current->next;
	} while (current != NULL);

	//Remove last newline (last line on body doesnt need newline (specs))
	char *p = body;
	p[strlen(p) - 1] = 0;

    //Connect to the server
	syslog(LOG_INFO, "Connecting to server");
	struct addrinfo* results = get_sockaddr(options[3], false);
	int sockfd = connection(results, false);

	//Issue an AUTH request
	char authMsg[strlen("AUTH\n") + strlen("Username:") + sizeof(options[0]) + strlen("\n") + strlen("Password:") + sizeof(options[1]) + strlen("\n\n")];
	strcpy(authMsg, "AUTH\n");

	//Concatinate the username and password into the AUTH message
	sprintf(authMsg, "AUTH\nUsername:%s\nPassword:%s\n\n", options[0], options[1]);

	//Buffer to store received message, leaving space for the NULL terminator
	char bufferAuthRequest[AUTHBUFFERLENGTH];
	
	syslog(LOG_DEBUG, "Sending Credentials");

	//Send the AUTH message to the server
	sendToServer(sockfd, authMsg, bufferAuthRequest);
	
	//Check if the first 3 digits returned are not accepting code
	if (strncmp(bufferAuthRequest, "200", 3) != 0)
	{
		//If so exit the program
		syslog(LOG_INFO, "Connecting Failed");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_INFO, "Authentication successful");

	//Extract the Token part of the returned string
	strcpy(token,strstr(bufferAuthRequest, "Token:"));
	token[strlen(token) - 5] = '\0';

	//Obtain the length of the body and store it in str (temp variable)
	sprintf(str, "%d", strlen(body));

	//Create a LIST request
	char msgList[strlen("LIST\n") + sizeof(token) + strlen("Length:") + sizeof(str) + strlen("\n\n") + sizeof(body)];

	//Concatinate the Header, token, length and end header. Then concatinate body with header.
	sprintf(msgList, "LIST\n%sLength:%s\n\n%s", token, str, body);

	//Buffer to store received message, leaving space for the NULL terminator
	char bufferListRequest[LISTBUFFERLENGTH];

	syslog(LOG_INFO, "Uploading file list");

	//Send the LIST request to the server
	sendToServer(sockfd, msgList, bufferListRequest);;

    syslog(LOG_INFO, "Server requested the following files:\n%s", strstr(bufferListRequest, "\n\n") + 2);

	// Close the connection
	close(sockfd);

	//Prepare the token (remove title and \n)
	token[strlen(token)-1] = '\0';

	sendFiles(bufferListRequest, options[5], options[6], token + LENGTH_OF_TOKEN_TITLE, listRoot);

	//Print the list of files requested by the server. If the server requests no files, print an appropriate message.
	exit(EXIT_SUCCESS);

}//End of main method

// sendToServer()
// Description:   Sends the msg thru the socket sockfd and places reponse in buffer
//
// sockfd:        The socket number
// msg:           The message you wish to send
// buffer:        A place to store the reply from the server
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

void sendFiles(char* filelist, char* address, char* port, char* token, hfs_entry* listRoot)
{
  	ctrl_message* response; // Response returned by the server
	host server;            // Address of the server

  	//Create a socket to listen on port specified
  	int sockfd = create_client_socket(address, port, &server);

	//Find the start of the list of files
	filelist = (strstr(filelist, "\n\n") + 2);

	//Create a tokenizer to deal with one filename at a time
	char* tokenizer;

	//Seperate one line form the file list
	tokenizer = strtok(filelist, "\n");

	//Send a type 1 control message with the nextSeq number and the rest of the files details
	message* ctrlMsg = createCtrlMessage(tokenizer, token, listRoot);

	ctrl_message* tempmsg = (ctrl_message*)ctrlMsg;

	char* filename = calloc(sizeof(char),tempmsg->flength);
	toString(&filename, tempmsg->filename, tempmsg->flength);

	printf("Recieved Filename: [%s]\n", filename);

	//Send it and free its memory
  	int retval = send_message(sockfd, ctrlMsg, &server);
  	free(ctrlMsg);

	if (retval == -1)
	{
		close(sockfd);
		perror("Unable to send to socket");
		exit(EXIT_FAILURE);
	}

	response = (ctrl_message*)receive_message(sockfd, &server);

	printf("%u\n", response->checksum);

	close(sockfd);
	exit(EXIT_SUCCESS);
	//Send the data message containing the first chunk of the file, and wait for the approperiate ACK
	
	//If the server's response contains an error, print an error message (token invalid)

	//If there is more data, increment dataSeq and loop ^

	//If there is another file to send, increment nextSeq and loop ^^ 	
	
	//Once all files have been transmitted send a type 2 control message and wait for an ACK
}

message* createCtrlMessage(char* filename, char* token, hfs_entry* listRoot)
{
	ctrl_message* msg = (ctrl_message*)create_message();
	char* details[2] = { 0 };			//Declare an array of 2 details to be populated
	getDetails(filename, details, listRoot);

	msg->length = htons(SIZE_OF_CONTROLMSG + strlen(filename));
	msg->type = 0;
	msg->numSeq = 0;
	msg->flength = htons(strlen(filename));
	touint32(details[1], &msg->filesize);
	touint32(details[0], &msg->checksum);
	touint32(token, &msg->token[0]);
	touint32(filename, &msg->filename[0]);

	printf("Expected filename: [%s]\nExpected flength: [%d]\nExpected Filesize: [%s]\nExpected Checksum: [%s]\nExpected Token: [%s]\n", filename, strlen(filename), details[1], details[0], token);

	return (message*)msg;
}//end of createCtrlMessage

void touint32(char* string, uint32_t field[])
{
	uint32_t buffer[1];
	int element = 0;
	int bitsRead = 0;
	int totalbits = strlen(string) * SIZE_OF_BYTE;

	while (bitsRead < totalbits)
	{
		memcpy(&buffer, string + bitsRead, 4);
		field[element] = htonl(*buffer);
		element++;
		bitsRead += 32;
	}

}//End of touint32 method

void toString(char** string, uint32_t field[], int length)
{
	char buffer[4];
	int element = 0;
	int bitsRead = 0;

	while (bitsRead <= length)
	{
		long val = ntohl(field[element]);
		memcpy(&buffer, &val, 4);

		strcpy(*string + bitsRead, buffer);

		element++;
		bitsRead += 32;
	}

}//End of toString method

void getDetails(char* filename, char* details[], hfs_entry* listRoot)
{
	//Set a pointer to the root/head of the linked list
	hfs_entry* current = listRoot;

	char buffer[1000];
	char buffer2[1000];

	//for each entry in listRoot
	do
	{
		//Compare if the current is the specified filename
		if (strcmp(current->rel_path, filename))
		{

			sprintf(buffer, "%ul", current->crc32);
			details[0] = buffer;

			sprintf(buffer2, "%ld", getFilesize(current->abs_path));
			details[1] = buffer2;
		}

		//Move on to the next file
		current = current->next;
	} while (current != NULL);

}//End of getDetails method

long getFilesize(char* filename)
{
	FILE *fp;

	fp = fopen(filename, "rb");

	fseek(fp, 0, SEEK_END);
	return ftell(fp);
}//End of getFilesize method



