#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv)
{
	 bool verbose = false;
	 char* log_filename;
	 int i;

	 // Iterate over the command line arguments
	 for (i = 1; i < argc; ++i)
	 {
		 // Check for the --verbose / -v flag
		 if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0)
		 {
			verbose = true;
		 }

		// Check for the --log / -l flag
		else if (strcmp(argv[i], "--log") == 0 || strcmp(argv[i], "-l") == 0)
		{
			// Ensure that an argument was provided
			if (i + 1 == argc) 
			{
				fprintf(stderr, "The --log / -l option requires an argument\n");
				exit(EXIT_FAILURE);
			}
			else 
			{
				log_filename = argv[++i];
			}
		}
	 }

	 printf("Verbose : %s\n", verbose ? "true" : "false");
	 printf("Log : %s\n", log_filename);

	 exit(EXIT_SUCCESS);
}
