/*
Author: Gurpreet Singh
Description: This class implements the CRC32 method from zlib
*/

#include "checksum.h"

/* Implements CRC32 on buffer with size length*/
uLong computeChecksum(const Bytef* buffer, long length)
{
	//Declare a crc32 uLong
	uLong crc = crc32(0L, Z_NULL, 0);
	
	//Calls the zlib function to compute crc32
	crc = crc32(crc, buffer, (unsigned int)length);
	
	return crc;
}//End of computeChecksum method

