#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "udp_sockets.h"
#include "udp_server.h"

int main()
{
  message* msg;   // Message received
  host source;   // Source of the message received

  // Create a socket to listen on port 5000
  int sockfd = create_server_socket("5000");

  // Read the next message
  msg = receive_message(sockfd, &source);

  if (msg != NULL)
  {
    // Add NULL terminator
    msg->buffer[msg->length] = '\0';

    printf("Message received from %s: %s\n", source.friendly_ip, msg->buffer);

    // Echo the message back to the client
    if (send_message(sockfd, msg, &source) == -1)
    {
      free(msg);
      perror("Unable to send to socket");
      exit(EXIT_FAILURE);
    }

  }
  
  // Free the memory allocated to the message
  free(msg);

  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}

