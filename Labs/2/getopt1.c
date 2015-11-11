#include <stdio.h>
#include <stdlib.h>
#define NAME_LENGTH 20
int main()
{
	 char name[NAME_LENGTH];
	 printf("Please enter your name: ");
	 
	 if (fgets(name, NAME_LENGTH, stdin) != NULL)
	 {
		printf("Nice to meet you, %s", name);
	 }

	 exit(EXIT_SUCCESS);
}