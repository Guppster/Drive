
#include <stdio.h>
#include <string.h>
#include <hfs.h>
#include "../common/parse.h"
#include "../common/networking.h"

void sendToServer(int sockfd, char* msg, char* buffer);