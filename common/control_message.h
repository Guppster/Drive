#include <stdint.h>

#ifndef CTRL_MESSAGE_H
#define CTRL_MESSAGE_H

typedef struct
{
	int length;
	uint8_t type;                
	uint8_t numSeq;        
	uint16_t flength;           
	uint32_t filesize;                    
	uint32_t checksum;          
	uint32_t token[4];
	uint32_t filename[361];
}ctrl_message;

#endif
