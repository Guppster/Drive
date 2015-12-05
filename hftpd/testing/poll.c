#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "udp_sockets.h"
#include "udp_server.h"

int main()
{
  message* msg;   // Message read from the client
  host client;    // Client's address
  int retval;       // Return value from poll

  // Create a socket to listen on port 5000
  int sockfd = create_server_socket("5000");

  // We will poll sockfd for the POLLIN event
  struct pollfd fd = {
    .fd = sockfd,
    .events = POLLIN
  }; 

  // Poll the socket for 10 seconds
  retval = poll(&fd, 1, 10000);

  if (retval == 1 && fd.revents == POLLIN)
  {
    // Read the waiting message
    msg = receive_message(sockfd, &client);

    // Add NULL terminator and print the message
    msg->buffer[msg->length] = '\0';
    printf("Message received from %s: %s\n", client.friendly_ip, msg->buffer);

    free(msg);
  }
  else
  {
    puts("No message received in 10s.  Exiting.");
  }

  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}
