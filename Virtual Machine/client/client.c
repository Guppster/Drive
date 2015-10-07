#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	struct dirent *pEnt;
	DIR *pDir;

	if (argc != 3) // argc should be 3 for correct execution
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		//Open the directory and store a pointer to it's location
		pDir = opendir(argv[1]);

		if (pDir == NULL) 
		{
			printf("Cannot open directory '%s'\n", argv[1]);
			exit(EXIT_FAILURE);
		}

		while ((pEnt = readdir(pDir)) != NULL)
		{

			if (strcmp(pEnt->d_name, ".") == 0 || strcmp(pEnt->d_name, "..") == 0)
			{
				continue;
			}
			else
			{
				printf("[%s]\n", pEnt->d_name);
			}
			
		}

		closedir(pDir);

		//Scans the directory argv[1] recursively (opendir, readdir, closedir)
			//Compute a CRC-32 checksum for each file

		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;
}
