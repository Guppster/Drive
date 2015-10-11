#include "checksum.h"

uLong computeChecksum(char* buffer, long length)
{
	uLong crc = crc32(0L, Z_NULL, 0);

	while (read_buffer(buffer, length) != 0)		//0 is suppose to be EOF but EOF is unrecognized
	{
	   crc = crc32(crc, buffer, length);
	}

	return crc;
}//End of computeChecksum method

