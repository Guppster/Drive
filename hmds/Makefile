CC      = gcc
CFLAGS  = -g -O0 -Wall -Werror -std=gnu11

server: server.c server.h
	$(CC) $(CFLAGS) $< -o $@ -lhdb

clean:
	rm -f *.o server
