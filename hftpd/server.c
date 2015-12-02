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

message* create_response_message(ctrl_message* request)
{
  int i;

  // Create a response message and initialize it
  ctrl_message* response = (ctrl_message*)create_message(); 

  // Return the dynamically-allocated message
  return (message*)response;  
}

int main(int argc, char *argv[])
{
  ctrl_message* request;			// Client's request message
  message* response;				// Server response message
  host client;						// Client's address
  char* options[4] = { 0 };			//Declare an array of 4 options to be read in from command line

  parseInput(argc, argv, options, 2);

  // Create a socket to listen on port 5000
  int sockfd = create_server_socket(options[1]);

  // Read the request message and generate the response
  request = (calc_message*)receive_message(sockfd, &client);
  response = create_response_message(request);

  // Send the response and free the memory allocated to the messages
  send_message(sockfd, response, &client);
  free(request);
  free(response);

  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}
