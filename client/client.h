
#include <stdio.h>
#include <string.h>
#include <hfs.h>
#include "../common/parse.h"
#include "../common/networking.h"
#include "../common/message.h"
#include "../common/udp_client.h"
#include "../common/udp_sockets.h"

#define LISTBODYLENGTH 10000
#define AUTHBUFFERLENGTH 50
#define LISTBUFFERLENGTH 10000
#define TEMPSTRLENGTH 1000
#define SIZE_OF_BYTE 8
#define SIZE_OF_CONTROLMSG 28
#define LENGTH_OF_TOKEN_TITLE 6


int main (int, char*[]);
void sendToServer(int sockfd, char* msg, char* buffer);
void sendFiles(char* filelist, char* address, char* port, char* token, hfs_entry* listRoot);
message* createCtrlMessage(char* filename, char* token, hfs_entry* listRoot);
void getDetails(char* filename, char* details[], hfs_entry* listRoot);
long getFilesize(char* filename);