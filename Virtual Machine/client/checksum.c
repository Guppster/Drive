#include "checksum.h"

uLong computeChecksum(char* buffer, long length)
{
	uLong crc = crc32(0L, Z_NULL, 0);
	  
	crc = crc32(crc, buffer, (unsigned int)length);
	
	return crc;
}//End of computeChecksum method

