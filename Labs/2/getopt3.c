#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

int main(int argc, char** argv)
{
	int c;
	bool verbose = false;
	char* log_filename = NULL;

	while ((c = getopt(argc, argv, "vl:")) != -1)
	{
		switch (c)
		{
		case 'v':
			verbose = true;
			break;

		case 'l':
			log_filename = optarg;
			break;

		case '?':
			// Error message already printed by getopt -- we'll just exit
			exit(EXIT_FAILURE);
			break;
		}
	}

	printf("Verbose : %s\n", verbose ? "true" : "false");
	printf("Log : %s\n", log_filename);
	for (i = optind; i < argc; ++i)
	{
		printf("Processing file %s\n", argv[i]);
	}

	exit(EXIT_SUCCESS);}