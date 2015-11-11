// #includes omitted for brevity
int main(int argc, char** argv)
{
	int c, i;
	static int verbose_flag = 0;
	char* log_filename = NULL;
	while (1)
	{
		static struct option long_options[] =
		{
			{ "verbose", no_argument, &verbose_flag, 1 },
			{ "log", required_argument, 0, 'l' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "vl:", long_options, &option_index);
		// If we've reached the end of the options, stop iterating
		if (c == -1)
			break;

		switch (c)
		{
		case 'l':
			log_filename = optarg;
			break;
		case 'v':
			verbose_flag = 1;
			break;
		case '?':
			// Error message already printed by getopt_long -- we'll just exit
			exit(EXIT_FAILURE);
			break;
		}
	}

	printf("Verbose : %s\n", verbose_flag ? "true" : "false");
	printf("Log : %s\n", log_filename);

	for (i = optind; i < argc; ++i)
	{
		printf("Processing file %s\n", argv[i]);
	}

	exit(EXIT_SUCCESS);
}