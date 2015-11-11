/*
Author: Gurpreet Singh
Description: This is the main file, it operates fileIO.c and Checksum.c to achieve the final result of a test file containing names and CRCs
*/

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
	//Options

	//-s HOSTNAME / --server HOSTNAME
	//Specifies the server's hostname. If not given, default to localhost.

	//-p PORT / --port PORT
	//Specifies the server's port. If not given, default to 9000.
	
	//- d DIR / --dir DIR
	//Specifies the Hooli root directory.If not given, default to ~/ hooli.
	
	//- v / --verbose
	//Enable verbose output : set the syslog level to DEBUG.Otherwise, it should default to INFO.

	//Your client should also require two arguments : USERNAME and PASSWORD.
	
	static int verbose_flag = 0;
	int c;
	char* hostname = NULL;
	char* dir = NULL;
	char* port = NULL;
	char* name = NULL;
	char* pass = NULL;

	openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);

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
			hostname = optarg; 
			break;

		case 'd':
			dir = optarg;
			break;

		case 'o':
			port = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			exit(EXIT_FAILURE);
			break;
		}

	}

	if (optind == argc-2)
	{
			name = argv[optind++];
			pass = argv[optind++];
	}
	else
	{
		syslog(LOG_ERR, "No Username / Password specified");
		closelog();
		exit(EXIT_FAILURE);
	}

	printf("%s, %s, %s, %s, %s \n", name, pass, hostname, dir, port);
	exit(0);

}//End of main method