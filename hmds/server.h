#define AUTHMESSAGELENGTH 80
#define BUFFERLENGTH 4096
#define CREDENTIALSLENGTH 40
#define LISTBODYLENGTH 10000
#define LISTMSGLENGTH 11000
#include "../common/networking.h"
#include "../common/parse.h"
#include <string.h>

int wait_for_connection(int sockfd);
void handle_connection(int connectionfd, char* hostname);
