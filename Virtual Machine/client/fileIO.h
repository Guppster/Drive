#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>
#include "checksum.h"

//This method traverses directories recursively 
void traverse(char*, int, char*);

//Writes one entry to the specified file
void writeLineToFile(char*, uLong, char*);

//Cleans out the file
void cleanFilesCache(char*);

//Reads in the contents of the file
char* readFile(char*);

//Returns the length of the file
long getSizeOfFile(char*);