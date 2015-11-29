
#include <stdio.h>
#include <string.h>
#include <hfs.h>
#include "../common/parse.h"
#include "../common/networking.h"

#define LISTBODYLENGTH 10000
#define AUTHBUFFERLENGTH 50
#define LISTBUFFERLENGTH 10000
#define TEMPSTRLENGTH 1000

void sendToServer(int sockfd, char* msg, char* buffer);

typedef struct{
	unsigned int type;
	unsigned int numSeq;
	unsigned int length; 
	unsigned int fileSize; 
	unsigned int checksum; 
	unsigned int token;
	uint8_t filename[100];
}msgCtrl;
		
