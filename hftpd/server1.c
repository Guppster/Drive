#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "udp_sockets.h"
#include "udp_server.h"

uint32_t calculate_sum(uint8_t* message)
{
  uint8_t operand_count = message[0];   // Read operand count from message
  uint8_t* ptr = message + 8;           // Pointer to start of operands
  uint16_t next_operand;                // Next operand 
  uint32_t sum = 0;                     // Computed sum
  int i;

  // Iterate over the operands
  for (i = 0; i < operand_count; ++i, ptr += sizeof(uint16_t))
  {
    // Copy each 16-bit operand from the message and add it to the sum
    memcpy(&next_operand, ptr, sizeof(uint16_t));
    sum += next_operand;
  }

  // Return the computed sum
  return sum;
}

uint8_t* create_response_message(uint32_t result)
{
  // Create an 8-byte message
  uint8_t* message = (uint8_t*)malloc(8 * sizeof(uint8_t));

  // The message contains no operands
  message[0] = 0;

  // Copy the result into the array, starting at byte 4,
  // since we need to skip the Reserved field
  memcpy(message + 4,  &result, sizeof(uint32_t));

  // Return the dynamically-allocated message
  return message;
}

int main()
{
  uint32_t result;         // Result to be returned to the client
  struct sockaddr_in addr; // Source address and port
  socklen_t addr_len = sizeof(struct sockaddr_in); // Length of the addr structure
  uint8_t request[1024];   // Buffer to store client's request
  uint8_t* response;

  // Create a socket to listen on port 5000
  int sockfd = create_server_socket("5000");

  // Read the next message into buffer, storing the source address in addr
  recvfrom(sockfd, request, sizeof(request), 0, (struct sockaddr*)&addr, &addr_len);

  // Compute the sum from the client's request
  result = calculate_sum(request);
  response = create_response_message(result);

  // Send the 8-byte result to the client
  sendto(sockfd, response, 8, 0, (struct sockaddr*)&addr, addr_len);
  free(response);

  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}
