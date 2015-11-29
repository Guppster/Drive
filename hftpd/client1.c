#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "udp_sockets.h"
#include "udp_client.h"

uint8_t* create_calc_request(uint16_t op1, uint16_t op2, uint16_t op3, uint16_t op4)
{
  // Create a 16-byte message
  uint8_t* message = (uint8_t*)malloc(16 * sizeof(uint8_t));   

  // Store the operand count in the first byte of the message
  message[0] = 4;                                               

  // Copy the operands into the array, starting at byte 8,
  // since we need to skip the Reserved and Sum fields
  memcpy(message + 8,  &op1, sizeof(uint16_t));                  
  memcpy(message + 10, &op2, sizeof(uint16_t));                  
  memcpy(message + 12, &op3, sizeof(uint16_t));
  memcpy(message + 14, &op4, sizeof(uint16_t));

  // Return the dynamically-allocated message
  return message;                                                 
}

int main()
{
  host server;       // Address of the server
  uint8_t response[8]; // Response returned by the server
  uint32_t result;     // Result returned in server's response

  // Create a socket to listen on port 5000
  int sockfd = create_client_socket("localhost", "5000", &server);

  // Encode the message to be sent
  uint8_t* request = create_calc_request(10, 20, 30, 40);

  // Send the 16-byte request to the server and free its memory
  sendto(sockfd, request, 16, 0, (struct sockaddr*)&server.addr, server.addr_len);
  free(request);

  // Read the response from the server
  recvfrom(sockfd, response, sizeof(response), 0, NULL, NULL);

  // Extract the result from the response (starting at byte 4)
  memcpy(&result, response + 4, sizeof(uint32_t));

  // Print the result and close the socket
  printf("Result: %d\n", result);
  close(sockfd);

  exit(EXIT_SUCCESS);
}
