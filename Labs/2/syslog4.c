#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
int main(int argc, char** argv)
{
	FILE* file;
	openlog("fig3-11", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	syslog(LOG_DEBUG, "Program started");
	syslog(LOG_DEBUG, "Testing arguments");
	syslog(LOG_DEBUG, "Arguments passed to the program: %d", argc);
	// Check if a filename was passed as an argument
	if (argc != 2)
	{
		syslog(LOG_ERR, "No filename specified");
		closelog();
		exit(EXIT_FAILURE);
	}
	// If so, open the file
	syslog(LOG_DEBUG, "Opening file '%s'", argv[1]);
	file = fopen(argv[1], "r");
	// Ensure the file was opened successfully
	if (file == NULL)
	{
		syslog(LOG_ERR, "The specified file could not be opened");
		closelog();
		exit(EXIT_FAILURE);
	}
	syslog(LOG_INFO, "The file '%s' exists and was successfully opened!", argv[1]);
	close(file);
	closelog();
	exit(EXIT_SUCCESS);
}