#include <string.h>
#include "../common/parse.h"
#include "../common/networking.h"

int wait_for_connection(int sockfd);
void handle_connection(int connectionfd, char* hostname);
