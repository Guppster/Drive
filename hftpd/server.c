#include "server.h"

//0 for no error, 1 for error (invalid token)
message* create_response_message(int error, int seqNum)
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
	ctrl_message* requestctrl;	 					// Client's request message
	data_message* requestdata;	 					// Client's data message
	host client;									// Client's address
	int expectedSeqNum = 0;
	char* options[4] = { 0 };						// Declare an array of 4 options to be read in from command line
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
		requestctrl = (ctrl_message*)receive_message(sockfd, &client);

		//Check if the seqNum is what it should be
		if ((requestctrl->numSeq) != expectedSeqNum)
		{
			continue;
		}

		//int checksum = ntohl(request->checksum);
		//int filesize = ntohl(request->filesize);

		//Create a filename string of length read in
		char filename[requestctrl->flength];

		//Copy in the filename from the struct to local variable
		memcpy(filename, (char*)requestctrl->filename, requestctrl->flength);

		//Copy in the token from the struct to a local variable
		memcpy(token, (char*)requestctrl->token, 16);

		//Authenticate the read in token and retrieve a username
		username = hdb_verify_token(dbConnection, token);

		//Send back the control message response 
		if ((requestctrl->type == 1) && username != NULL)
		{
			//Create a directory with the passed in root directory, the user's username, and the filename + 2 for 2 '/'s
			char abs_directory[strlen(options[2]) + strlen(username) + strlen(filename) + 2];

			//Copy the outlined data into the directory field and seperate it with '/'s
			sprintf(abs_directory, "%s/%s/%s", options[2], username, filename);

			char* lastslash = strrchr(abs_directory, '/');

			char directory[lastslash - abs_directory];

			directory[lastslash - abs_directory] = '\0';

			strncpy(directory, abs_directory, lastslash - abs_directory);

			//Make a new directory and ignore errno value so do nothing if it already exists
			mkdir(directory, 0700);

			//open new file
			//fp = fopen(abs_directory, "wb");

			//update metadata

			//ACK(seq)
			message* respMsg = create_response_message(0, expectedSeqNum);

			// Send the response 
			send_message(sockfd, respMsg, &client);

			//expectedSeqNum = !expectedSeqNum
			expectedSeqNum = 1;

			//Free the request and response messages
			free(requestctrl);
			free(respMsg);

			do
			{
				// Read the request message and generate the response
				requestdata = (data_message*)receive_message(sockfd, &client);

				//ACK(seq)
				respMsg = create_response_message(0, expectedSeqNum);

				// Send the response 
				send_message(sockfd, respMsg, &client);

			} while ((requestdata->numSeq) != expectedSeqNum);

			//ACK(seq)
			respMsg = create_response_message(0, expectedSeqNum);

			// Send the response 
			send_message(sockfd, respMsg, &client);
			
			//expectedSeqNum = !expectedSeqNum
			expectedSeqNum = 0;

			//Write data to file
		}
		else
		{
			message* respMsg = create_response_message(0, expectedSeqNum);

			// Send the response 
			send_message(sockfd, respMsg, &client);

			//Free the request and response messages
			free(respMsg);
		}
	} while (1);

	// Close the socket
	close(sockfd);

	//Exit successfully
	exit(EXIT_SUCCESS);
}//End of main method
