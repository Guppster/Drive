#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <err.h>

void parseInput(int argc, char *argv[], char* result[], bool isClient)
{
	int c;
	int verbose_flag = 0;

	/* getopt_long stores the option index here. */
	int option_index = 0;

	if (isClient)
	{
		struct option long_options1[] =
		{
			{ "verbose", no_argument, &verbose_flag, 1 },
			{ "server", required_argument, 0, 's' },
			{ "dir", required_argument, 0, 'd' },
			{ "port", required_argument, 0, 'p' },
			{ "fserver", required_argument, 0, 'f' },
			{ "fport", required_argument, 0, 'o' },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "vs:d:p:", long_options1, &option_index);
	}
	else
	{
		struct option long_options2[] =
		{
			{ "verbose", no_argument, &verbose_flag, 1 },
			{ "server", required_argument, 0, 's' },
			{ "port", required_argument, 0, 'p' },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "vs:p:", long_options2, &option_index);
	}

	switch (c)
	{
	case 'v':
 		verbose_flag = 1;
		break;

	case 's':
		if (isClient)
			result[2] = optarg;
		else
			result[0] = optarg;
		break;

	case 'd':
		result[4] = optarg;
		break;

	case 'p':
		if (isClient)
			result[3] = optarg;
		else
			result[1] = optarg;
		break;

	case '?':
		/* getopt_long already printed an error message. */
		exit(EXIT_FAILURE);
		break;
	}

	if (isClient)
	{
		if (optind == argc - 2)
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
			result[4] = "/hooli";
		}
	
		if (result[5] == 0)
		{
			result[5] = "localhost";
		}
	
		if (result[6] == 0)
		{
			result[6] = "10000";
		}
	}
	else
	{
		if (result[0] == 0)
		{
			result[0] = "localhost";
		}

		if (result[1] == 0)
		{
			result[1] = "9000";
		}
	}

  if(verbose_flag == 0)
  {
    setlogmask(LOG_UPTO(LOG_INFO));
  }
}
