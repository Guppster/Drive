/*
Author: Gurpreet Singh
Description:
*/

#include "fileIO.h"

/* This method traverses all directories recursively in name and outputs a file with CRC checksums*/
void traverse(char* name, int lengthOfFoldierName, char* outputname)
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
			//Concatingate the name of the file on to the end of a tempstring
			strcpy(tempname, name);
			strcat(tempname, "/");
			strcat(tempname, dEnt->d_name);

			//Enter directory and recursivly repeat process
			traverse(tempname, lengthOfFoldierName, outputname);
		}
		else if (S_ISREG(fInfo.st_mode))	//If it is a regular file
		{
			//Copy the name to a temp variable that displays name in the correct format
			strcpy(formattedname, name);

			//Skip over the main directory's name 
			formattedname = formattedname + lengthOfFoldierName;

			//Concatinate the filename at the end
			strcat(formattedname, "/");
			strcat(formattedname, dEnt->d_name);

			//Concatinate the filename onto the end of another temp variable this time keeping the main directory in the string for reading purposes
			strcpy(full_path, name);
			strcat(full_path, "/");
			strcat(full_path, dEnt->d_name);
		
			//Write the final entry to file with the name and checksum
			writeLineToFile(formattedname, computeChecksum(readFile(full_path), getSizeOfFile(full_path)), outputname);
		}
	}

	//Close the directory
	closedir(pDir);
}//End of traverse method

//Formats and writes one line to the files.txt file. 
void writeLineToFile(char* fileName, uLong checksum, char* outputname)
{
	FILE *fileptr;
	fileptr = fopen(outputname, "ab");
	fprintf(fileptr, "%s,%lu\n", fileName, checksum);
	fclose(fileptr);
}//End of writeLineToFile method

//Empties the files.txt file
void cleanFilesCache(char* outputname)
{
	//Opens the file in write mode which overwrites any contents and then closes the file
	FILE *fileptr;
	fileptr = fopen(outputname, "w");
	fclose(fileptr);
}//End of cleanFilesCache method

//Reads and returns the contents of the file specified in the parameter
Bytef* readFile(char* filename)
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

	return (Bytef*)buffer;

	//Free the memory
	free(buffer);

}//End of readfile method

//Returns the size of the file specified in the parameter
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
