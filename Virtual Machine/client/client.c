/*
Author: Gurpreet Singh
Description: This is the main file, it operates fileIO.c and Checksum.c to achieve the final result of a test file containing names and CRCs
*/

#include "fileIO.h"

int main(int argc, char *argv[])
{
	if (argc != 3) // argc should be 3 for correct execution
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		//Delete the existing files.txt if it exists
		cleanFilesCache(argv[2]);

		//Write a comma seperated list of files in argv[1] and their CRC-32 checksums to the file argv[2]
		traverse(argv[1], strlen(argv[1]), argv[2]);
	}
	return 0;

}//End of main method
