/*
Author: Gurpreet Singh
Description: This is the header file containing definations for the checksum.c file
*/

#include <zlib.h>

uLong computeChecksum(const Bytef*, long);
char* readBuffer(char*, int);