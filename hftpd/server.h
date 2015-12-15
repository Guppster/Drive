#define TIME_WAIT 5

#include <arpa/inet.h>
#include "../common/message.h"
#include "../common/parse.h"
#include "../common/udp_server.h"
#include "../common/udp_sockets.h"
#include <errno.h>
#include <hdb.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>

static void _mkdir(const char *dir);