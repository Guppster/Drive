/*
Author: Gurpreet Singh
Description: This is the header file containing definations for the checksum.c file
*/

#include <zlib.h>

//This method computes the crc checksum and returns it
uLong computeChecksum(const Bytef*, long);
