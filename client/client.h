#define AUTHBUFFERLENGTH 50
#define LENGTH_OF_TOKEN_TITLE 6
#define LISTBODYLENGTH 10000
#define LISTBUFFERLENGTH 10000
#define SIZE_OF_BYTE 8
#define SIZE_OF_CONTROLMSG 28
#define SIZE_OF_DATA 1467
#define SIZE_OF_DATAMSG 4
#define TEMPSTRLENGTH 1000

#include "../common/message.h"
#include "../common/networking.h"
#include "../common/parse.h"
#include "../common/udp_client.h"
#include "../common/udp_sockets.h"
#include <hfs.h>
#include <stdio.h>
#include <string.h>


int main (int, char*[]);
long getFilesize(char* filename);
message* createCtrlMessage(char* filename, char* token, char* details[], int nextSeq, int type);
message* createDataMessage(char* filename, int nextSeq, int alreadyReadIn, int bytesToSend);
void getDetails(char* filename, char* details[], hfs_entry* listRoot);
void readInFile(char* buffer, char* filename, int alreadyReadIn, int bytesToRead);
void sendFiles(char* filelist, char* address, char* port, char* token, hfs_entry* listRoot);
void sendToServer(int sockfd, char* msg, char* buffer);
