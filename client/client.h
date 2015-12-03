
#include <stdio.h>
#include <string.h>
#include <hfs.h>
#include "../common/parse.h"
#include "../common/networking.h"
#include "../common/control_message.h"
#include "../common/udp_client.h"

#define LISTBODYLENGTH 10000
#define AUTHBUFFERLENGTH 50
#define LISTBUFFERLENGTH 10000
#define TEMPSTRLENGTH 1000

void sendToServer(int sockfd, char* msg, char* buffer);
void sendFiles(char* filelist, char* address, char* port, char* token, char* body);
message* createCtrlMessage(int type, char* filename, char* token, char* body);
char* getChecksumFromBody(char* filename, char* body);
