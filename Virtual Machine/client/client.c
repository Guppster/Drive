#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 3) // argc should be 3 for correct execution
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		//Scans the directory argv[1] recursively
			//compute a CRC-32 checksum for each file

		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;
}