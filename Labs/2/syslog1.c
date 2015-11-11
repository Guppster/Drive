#include <syslog.h>
#include <stdlib.h>
int main()
{
	openlog("fig3-7", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	syslog(LOG_NOTICE, "Hello Syslog!");
	syslog(LOG_INFO, "Here's an informational message.");
	syslog(LOG_WARNING, "Here's a warning.");
	syslog(LOG_ERR, "Here's an error.");
	syslog(LOG_DEBUG, "Here's some debugging information.\n");
	closelog();

	exit(EXIT_SUCCESS);
}