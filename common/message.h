#include <stdint.h>

#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct
{
	int length;
	uint8_t type;
	uint8_t numSeq;
	uint16_t flength;
	uint32_t filesize;
	uint32_t checksum;
	uint8_t token[16];
	uint8_t filename[1444];
}ctrl_message;

typedef struct
{
	int length;
	uint8_t type;
	uint8_t numSeq;
	uint16_t errCode;
}resp_message;

typedef struct
{
	int length;
	uint8_t type;
	uint8_t numSeq;
	uint16_t dataLen;
	uint8_t data[1467];
	uint8_t padding;
}data_message;

#endif
