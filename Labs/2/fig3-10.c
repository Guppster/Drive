#include <syslog.h>
#include <stdlib.h>
int main()
{
	int i = 42;
	char* str = "universe";
	openlog("fig3-10", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	syslog(LOG_INFO, "The answer to the %s is %d", str, i);
	closelog();
	exit(EXIT_SUCCESS);
}