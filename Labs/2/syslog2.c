#include <syslog.h>
#include <stdlib.h>
void b()
{
	syslog(LOG_DEBUG, "Entering b()");
	syslog(LOG_DEBUG, "Leaving b()");
}
void a()
{
	syslog(LOG_DEBUG, "Entering a()");
	b();
	syslog(LOG_DEBUG, "Leaving a()");
}
int main()
{
	openlog("fig3-9", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
	a();
	closelog();
	exit(EXIT_SUCCESS);
}