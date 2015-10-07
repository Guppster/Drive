#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	struct dirent *dEnt;
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

		while ((dEnt = readdir(pDir)) != NULL)
		{

			if (strcmp(dEnt->d_name, ".") == 0 || strcmp(dEnt->d_name, "..") == 0)
			{
				continue;
			}
			else
			{
				//If it is directory (d_type is always 0.... am i going to have to use stat(2) :o)
				if ((dEnt->d_type == DT_DIR))
				{
					
				}
				else if (!(dEnt->d_type == DT_DIR))	//If it isnt a directory
				{
					printf("[%s]\n", dEnt->d_name);
				}
				else
				{
					exit(EXIT_FAILURE);
				}
			}
		}

		closedir(pDir);

		//Scans the directory argv[1] recursively (opendir, readdir, closedir)
			//Compute a CRC-32 checksum for each file

		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;
}
