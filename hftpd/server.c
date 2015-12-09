#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "../common/control_message.h"
#include "../common/udp_sockets.h"
#include "../common/udp_server.h"
#include "../common/parse.h"
#include "server.h"

message* create_ctrl_response_message(ctrl_message* request)
{
  // Create a response message and initialize it 
  ctrl_message* response = (ctrl_message*)create_message(); 

  // Return the dynamically-allocated message
  return (message*)response;  
}

int main(int argc, char *argv[])
{
  ctrl_message* request;	 		// Client's request message
 // message* response;				// Server response message
  host client;						// Client's address
  char* options[4] = { 0 };			// Declare an array of 4 options to be read in from command line
  int expectedSeqNum = 0;

  parseInput(argc, argv, options, 2);

  // Create a socket to listen on port specified 
  int sockfd = create_server_socket(options[0]);
 
  //For every file do this
  do
  {
	  // Read the request message and generate the response
	  request = (ctrl_message*)receive_message(sockfd, &client);

	  char* filename = calloc(sizeof(char), request->flength);
	  toString(&filename, request->filename, request->flength);

	  printf("Filename: %s\n", filename);

	  //Check if the seqNum is what it should be
	  if ((request->numSeq) != expectedSeqNum)
	  {
		  continue;
	  }

	  if (request->type == 1)
	  {

	  }

	  //close previous file

	  //update metadata

	  //open new file

	  //ACK(seq)

	  //expectedSeqNum = !expectedSeqNum

	  //response = create_ctrl_response_message(request);

	  // Send the response and free the memory allocated to the messages
	  //send_message(sockfd, response, &client);
	  //free(request);
	  //free(response);
  
  } while (1);
  
  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}

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
