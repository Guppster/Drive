#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>
#include "checksum.h"

void traverse(char* , int );
int writeToFile(char*);
char* readFile(char*);
long getSizeOfFile(char*);