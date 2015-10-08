#include "checksum.h"

uLong computeChecksum(char* buffer, size_t length)
{
	uLong crc = crc32(0L, Z_NULL, 0);

	while (read_buffer(buffer, length) != EOF) 
	{
	   crc = crc32(crc, buffer, length);

	}

	if (crc != original_crc) 
		error();

	return crc;
}//End of computeChecksum method

