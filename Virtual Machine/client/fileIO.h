#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void traverse(char* name, int lengthOfFoldierName);
int writeToFile(char*);
int readFile(char*);