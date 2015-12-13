#include "server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//0 for no error, 1 for error (invalid token)
message* create_ctrl_response_message(ctrl_message* request, int error, int seqNum)
{
	// Create a response message and initialize it 
	resp_message* response = (resp_message*)create_message();

	response->length = 4;
	response->type = 255;
	response->numSeq = seqNum;
	response->errCode = error;		//maybe ntohs?

	// Return the dynamically-allocated message
	return (message*)response;
}

int main(int argc, char *argv[])
{
	//FILE *fp;
	//FILE *prevfp;
	ctrl_message* request;	 			// Client's request message
	host client;						// Client's address
	int expectedSeqNum = 0;
	char* options[4] = { 0 };			// Declare an array of 4 options to be read in from command line
	char* username = calloc(30,sizeof(char));
	char token[17];
	token[16] = '\0';

	//Obtain the arguments from command line
	parseInput(argc, argv, options, 2);

	//Create a DB Connection
	hdb_connection* dbConnection = hdb_connect(options[1]);

	// Create a socket to listen on port specified 
	int sockfd = create_server_socket(options[0]);

	//For every file do this
	do
	{
		// Read the request message and generate the response
		request = (ctrl_message*)receive_message(sockfd, &client);

		//int checksum = ntohl(request->checksum);
		//int filesize = ntohl(request->filesize);

		char filename[request->flength];

		memcpy(filename, (char*)request->filename, request->flength);
		memcpy(token, (char*)request->token, 16);

		username = hdb_verify_token(dbConnection, token);

		char directory[strlen(options[2]) + strlen(username) + strlen(filename) + 2];
		
		sprintf(directory, "%s/%s/%s", options[2], username, filename);

		printf("[%s]\n", directory);

		//Check if the seqNum is what it should be
		if ((request->numSeq) != expectedSeqNum)
		{
			//continue;
		}

		//Send back the control message response 
		if ((request->type == 1) && username != NULL)
		{
			//close previous file (why not close at end of data recieving?)
			//fclose(prevfp);

			//update metadata

			//open new file
			//fp = fopen(filename, "wb");

			//ACK(seq)
			message* respMsg = create_ctrl_response_message(request, 0, expectedSeqNum);

			//expectedSeqNum = !expectedSeqNum
			expectedSeqNum = 1;

			// Send the response and free the memory allocated to the messages
			send_message(sockfd, respMsg, &client);
			free(request);
			free(respMsg);
		}
		else
		{
			message* respMsg = create_ctrl_response_message(request, 0, expectedSeqNum);

			// Send the response and free the memory allocated to the messages
			send_message(sockfd, respMsg, &client);
			free(request);
			free(respMsg);

		}

	} while (1);

	// Close the socket
	close(sockfd);

	exit(EXIT_SUCCESS);
}
