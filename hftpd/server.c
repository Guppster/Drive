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

void handler(int s) 
{
	printf("Caught signal %d\n", s);
	exit(1);

}

int main(int argc, char *argv[])
{
	//Open a syslog for logging purposes
	openlog("HFTPD", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);

	FILE *fp;
	//FILE *prevfp;
	ctrl_message* requestctrl;	 					// Client's request message
	data_message* requestdata;	 					// Client's data message
	host client;									// Client's address
	int expectedSeqNum = 0;
	char* options[4] = { 0 };						// Declare an array of 4 options to be read in from command line
	char* username = calloc(30, sizeof(char));
	char token[17];
	token[16] = '\0';

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	//Obtain the arguments from command line
	parseInput(argc, argv, options, 2);

	int TIME_WAIT = atoi(options[3]);

	//Create a DB Connection
	hdb_connection* dbConnection = hdb_connect(options[1]);

	// Create a socket to listen on port specified 
	int sockfd = create_server_socket(options[0]);

	syslog(LOG_INFO, "Connection Established");

	//For every file do this
	do
	{
		syslog(LOG_DEBUG, "Waiting for file control message");

		// Read the request message and generate the response
		requestctrl = (ctrl_message*)receive_message(sockfd, &client);

		syslog(LOG_DEBUG, "File control data recieved");

		//Check if the seqNum is what it should be
		if ((requestctrl->numSeq) != expectedSeqNum)
		{
			syslog(LOG_DEBUG, "Sequence number was incorrect, skipping");
			continue;
		}

		syslog(LOG_DEBUG, "Sequence number was correct");

		int checksum = ntohl(requestctrl->checksum);
		//int filesize = ntohl(requestctrl->filesize);

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
			syslog(LOG_DEBUG, "Token verified. Username valid");

			//Create a directory with the passed in root directory, the user's username, and the filename + 2 for 2 '/'s
			char abs_directory[strlen(options[2]) + strlen(username) + strlen(filename) + 2];

			//Copy the outlined data into the directory field and seperate it with '/'s
			sprintf(abs_directory, "%s/%s/%s", options[2], username, filename);

			char* lastslash = strrchr(abs_directory, '/');

			char directory[lastslash - abs_directory];

			directory[lastslash - abs_directory] = '\0';

			strncpy(directory, abs_directory, lastslash - abs_directory);

			syslog(LOG_DEBUG, "Making directory: [%s]", directory);

			//Make a new directory and ignore errno value so do nothing if it already exists
			_mkdir(directory);

			//open new file
			fp = fopen(abs_directory, "wb");

			if (fp == NULL)
			{
				syslog(LOG_DEBUG, "Failed to open file");
				exit(EXIT_FAILURE);
			}

			//update metadata
			//Make a hdb_record
			hdb_record* record = malloc(sizeof(hdb_record));

			//Populate the hdb_record fields
			record->username = username;
			record->filename = filename;

			//Convert the checksum to a uppercase hexidecimal string from int
			char tempchecksum[50];
			sprintf(tempchecksum, "%X", checksum);

			//Populate the hdb_record checksum field
			record->checksum = tempchecksum;

			syslog(LOG_DEBUG, "Updating Redis Metadata");

			//Store file metadata (dont store in testing)
			hdb_store_file(dbConnection, record);

			syslog(LOG_DEBUG, "ACK'ing Control Message");

			//ACK(seq)
			message* respMsg = create_response_message(0, expectedSeqNum);

			// Send the response 
			send_message(sockfd, respMsg, &client);

			//expectedSeqNum = !expectedSeqNum
			expectedSeqNum = (expectedSeqNum == 1) ? 0 : 1;

			//Free the request and response messages
			free(requestctrl);
			free(respMsg);

			do
			{
				do
				{
					syslog(LOG_DEBUG, "Recieving a data message. Expected Seq = [%d]", expectedSeqNum);

					// Read the request message and generate the response
					requestdata = (data_message*)receive_message(sockfd, &client);

					//If we get a data message and it's not the right seq number, ACK(seq) but dont increment expectedSeq
					if (requestdata->type == 3 && ((requestdata->numSeq) != expectedSeqNum))
					{
						syslog(LOG_DEBUG, "ACK'ing Data Message with the wrong sequence number");

						//ACK(seq)
						respMsg = create_response_message(0, expectedSeqNum);

						// Send the response 
						send_message(sockfd, respMsg, &client);
					}

				} while ((requestdata->numSeq) != expectedSeqNum);		//If we get the right sequence number get out the reading loop

				//If we got a data message
				if (requestdata->type == 3)
				{
					syslog(LOG_DEBUG, "ACK'ing Data Message");

					//ACK(seq) the data message
					respMsg = create_response_message(0, expectedSeqNum);

					// Send the response 
					send_message(sockfd, respMsg, &client);

					//expectedSeqNum = !expectedSeqNum
					expectedSeqNum = (expectedSeqNum == 1) ? 0 : 1;

					syslog(LOG_DEBUG, "Writing To File, [%d] bytes", requestdata->dataLen);

					//Write data to file
					fwrite(requestdata->data, 8, requestdata->dataLen, fp);
				}
			} while (requestdata->type == 3);		//If the message read in wasnt a data message, we are done reciving data
		}
		else
		{
			syslog(LOG_DEBUG, "Incorrect message type or username");

			message* respMsg = create_response_message(0, expectedSeqNum);

			// Send the response 
			send_message(sockfd, respMsg, &client);

			//Free the request and response messages
			free(respMsg);
		}
	} while (requestdata->type == 1);

	syslog(LOG_DEBUG, "Type 2 control message recieved");

	//Close the file
	fclose(fp);

	syslog(LOG_DEBUG, "ACK'ing Type 2 control message");

	//ACK(seq) the type 2 control message
	message* respMsg = create_response_message(0, expectedSeqNum);

	//Send the response 
	send_message(sockfd, respMsg, &client);

	// We will poll sockfd for the POLLIN event
	struct pollfd fd =
	{
		.fd = sockfd,
		.events = POLLIN
	};

	syslog(LOG_DEBUG, "Polling for TIME_WAIT");

	//Start TIME_WAIT timer
	//Poll the socket for TIME_WAIT seconds
	int retval = poll(&fd, 1, TIME_WAIT * 1000);

	if (retval == 1 && fd.revents == POLLIN)
	{
		syslog(LOG_DEBUG, "ACK'ing Type 2 control message");

		//ACK(seq) the type 2 control message
		respMsg = create_response_message(0, expectedSeqNum);

		//Send the response 
		send_message(sockfd, respMsg, &client);
	}

	// Close the socket
	close(sockfd);

	//Exit successfully
	exit(EXIT_SUCCESS);

}//End of main method


//We use this method to recursivly make directories
static void _mkdir(const char *dir) 
{
	char tmp[PATH_MAX];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);
	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;

	for (p = tmp + 1; *p; p++)
		if (*p == '/') 
		{
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}
