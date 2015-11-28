#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "udp_client.h"
#include "udp_sockets.h"

int main()
{
  host server;   // Server address
  message* msg;  // Message to send/receive

  // Create a socket for communication with the server
  int sockfd = create_client_socket("localhost", "5000", &server);

  // Create a message, and initialize its contents
  msg = create_message();
  msg->length = strlen("hello");
  memcpy(msg->buffer, "hello", msg->length);

  // Send the message to the server, and free its memory
  int retval = send_message(sockfd, msg, &server);
  free(msg);

  // If we couldn't send the message, exit the program
  if (retval == -1)
  {
    close(sockfd);
    perror("Unable to send to socket");
    exit(EXIT_FAILURE);
  }

  // Read the server's reply
  msg = receive_message(sockfd, &server);

  if (msg != NULL)
  {
    // Add NULL terminator and print reply
    msg->buffer[msg->length] = '\0';
    printf("Reply from server %s: %s\n", server.friendly_ip, msg->buffer);

    // Free the memory allocated to the message
    free(msg);
  }

  // Close the socket
  close(sockfd);
  exit(EXIT_SUCCESS);
}
