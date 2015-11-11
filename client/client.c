/*
Author: Gurpreet Singh
Description: This is the main file
*/

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>

static int verbose_flag = 0;

void parseInput(int argc, char *argv[], char* result[]);

int main(int argc, char *argv[])
{
	openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	char* options[5] = { 0 };
	parseInput(argc, argv, options);

}//End of main method

void parseInput(int argc, char *argv[], char* result[])
{
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"verbose", no_argument, &verbose_flag, 1},
			{"server",	required_argument,	0,	's'},
			{"dir",		required_argument,	0,	'd'},
			{"port",	required_argument,	0,	'o'},
			{ 0, 0, 0, 0 }
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "vs:d:o:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 'v':
			verbose_flag = 1;
			break;
		
		case 's':
			result[2] = optarg; 
			break;

		case 'd':
			result[4] = optarg;
			break;

		case 'o':
			result[3] = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			exit(EXIT_FAILURE);
			break;
		}

	}

	if (optind == argc-2)
	{
			result[0] = argv[optind++];
			result[1] = argv[optind++];
	}
	else
	{
		syslog(LOG_ERR, "No Username / Password specified");
		closelog();
		exit(EXIT_FAILURE);
	}

	if (result[2] == 0)
	{
		result[2] = "localhost";
	}
	
	if (result[3] == 0)
	{
		result[3] = "9000";
	}

	if (result[4] == 0)
	{
		result[4] = "~/hooli";
	}
	
	exit(0);
}