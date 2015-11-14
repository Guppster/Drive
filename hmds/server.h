#include <string.h>
#include "../common/parse.h"
#include "../common/networking.h"

#define LISTBODYLENGTH 10000
#define CREDENTIALSLENGTH 40
#define AUTHMESSAGELENGTH 80
#define LISTMSGLENGTH 11000
#define BUFFERLENGTH 4096

int wait_for_connection(int sockfd);
void handle_connection(int connectionfd, char* hostname);
