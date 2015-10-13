#include "fileIO.h"

int main(int argc, char *argv[])
{
	if (argc != 3) // argc should be 3 for correct execution
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		cleanFilesCache(argv[2]);
		traverse(argv[1], strlen(argv[1]), argv[2]);
		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;

}
