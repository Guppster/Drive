#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct dirent *dEnt;
	struct stat fInfo;
	DIR *pDir;
	char full_path[512];						//Allocate enough space to hold a directory path

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
			
		}

		while ((dEnt = readdir(pDir)) != NULL)
		{
			//Exclude itself and previous directory
			if (strcmp(dEnt->d_name, ".") == 0 || strcmp(dEnt->d_name, "..") == 0)
			{
				continue;
			}

			//Obtain the current working directory and store in full_path
			getcwd(full_path, sizeof(full_path));
			strcat(full_path, "/");
			strcat(full_path, argv[1]);
			strcat(full_path, "/");

			//Concatinate the name of the retrieved object with the full path for use in stat()
			strcat(full_path, dEnt->d_name);

			//Obtain information about the file
			if (lstat(full_path, &fInfo) == -1)
			{
				perror(dEnt->d_name);
				exit(EXIT_FAILURE);
			}

			//If it is directory
			if (S_ISDIR(fInfo.st_mode))
			{
				printf("DIRECTORY");
				printf("[%s]\n", dEnt->d_name);
			}
			else if (S_ISREG(fInfo.st_mode))	//If it is a regular file
			{
				printf("REGULAR FILE");
				printf("[%s]\n", dEnt->d_name);
			}
			else
			{
				exit(EXIT_FAILURE);
			}
		}

		closedir(pDir);

		//Scans the directory argv[1] recursively (opendir, readdir, closedir)
			//Compute a CRC-32 checksum for each file

		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;
}
