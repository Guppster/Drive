#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void traverse();

int main(int argc, char *argv[])
{
	if (argc != 3) // argc should be 3 for correct execution
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		traverse(argv[1], strlen(argv[1]));
		//Write a comma seperated list of files and their CRC-32 checksums to the file argv[2]
	}
	return 0;
}

void traverse(char* name, int lengthOfFoldierName)
{
	struct dirent *dEnt;
	struct stat fInfo;
	DIR *pDir;
	char full_path[1024];						//Allocate enough space to hold a directory path 
	char tempname[1024];						//Allocate identical space to full_path to create a copy to reformat the path for the recursive process
	char* formattedname;

	getcwd(full_path, sizeof(full_path));

	//Open the directory and store a pointer to it's location
	pDir = opendir(name);

	if (pDir == NULL)
	{
		printf("Cannot open directory '%s'\n", name);
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
		strcat(full_path, name);
		strcat(full_path, "/");

		//Concatinate the name of the retrieved object with the full path for use in stat()
		strcat(full_path, dEnt->d_name);

		//Obtain information about the file and check for error
		if (lstat(full_path, &fInfo) == -1)
		{
			//Display the error and continue
			perror(dEnt->d_name);
			continue;
		}

		//If it is directory
		if (S_ISDIR(fInfo.st_mode))
		{
			strcpy(tempname, name);
			strcat(tempname, "/");
			strcat(tempname, dEnt->d_name);

			//Enter directory and recursivly repeat process
			traverse(tempname, lengthOfFoldierName);
		}
		else if (S_ISREG(fInfo.st_mode))	//If it is a regular file
		{
			formattedname = name + lengthOfFoldierName;
			printf("%s/%s\n", formattedname, dEnt->d_name);
		
			//Compute a CRC-32 checksum 
		}

	}

	closedir(pDir);
}//End of traverse method



