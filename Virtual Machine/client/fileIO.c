#include "fileIO.h"

void traverse(char* name, int lengthOfFoldierName)
{
	struct dirent *dEnt;
	struct stat fInfo;
	DIR *pDir;
	char full_path[1024];						//Allocate enough space to hold a directory path 
	char tempname[1024];						//Allocate identical space to full_path to create a copy to reformat the path for the recursive process
	char* formattedname = malloc(1024);

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
		getcwd(full_path, sizeof(full_path));			//Puts "/vagrant/client" in full_path
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
			strcpy(formattedname, name);

			formattedname = formattedname + lengthOfFoldierName;
			strcat(formattedname, "/");
			strcat(formattedname, dEnt->d_name);
			printf("\nFilename:\t%s\n", formattedname);

			strcpy(full_path, name);
			strcat(full_path, "/");
			strcat(full_path, dEnt->d_name);

			printf("Contents:\t\t%s\n", readFile(full_path));
		
			computeChecksum(readFile(name),getSizeOfFile(name));
		}
	}

	closedir(pDir);
}//End of traverse method

int writeToFile(char* output)
{
	FILE *fileptr;
	fileptr = fopen("output.txt", "w");
	fprintf(fileptr, "%s" ,output);
	return 0;
}

char* readFile(char* filename)
{
	//Open file in read binary mode
	FILE *fileptr = fopen(filename, "rb");

	// Jump to the end of the file
	fseek(fileptr, 0, SEEK_END);

	// Get the current byte offset in the file          
	int filelen = ftell(fileptr);     

	// Jump back to the beginning of the file        
	rewind(fileptr);         

	// Enough memory for file + \0
	char* buffer = (char *)malloc((filelen+1)*sizeof(char)); 
	
	// Read in the entire file
	fread(buffer, filelen, 1, fileptr); 
	
	// Close the file  
	fclose(fileptr);           

	return buffer;

}//End of readfile method

long getSizeOfFile(char* fileName)
{
	//Declare a file pointer
	FILE *fileptr;

	//Open file in read binary mode
	fileptr = fopen(fileName, "rb");

	//Find end of file
	fseek(fileptr, 0L, SEEK_END);

	//store postion
	long size =  ftell(fileptr);

	// Close the file  
	fclose(fileptr); 

	return size;
}//End of getSizeOfFile method

size_t read_buffer(char* buffer, long length, char* filename)
{

	//Declare a file pointer
	FILE *fileptr;

	//Open file in read binary mode
	fileptr = fopen(filename, "rb");

	size_t temp = fread(buffer, 1, length, fileptr);

	// Close the file  
	fclose(fileptr); 

	return temp;
}