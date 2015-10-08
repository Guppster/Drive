CC      = gcc
CFLAGS  = -g -O0 -Wall -Werror -std=gnu11
LDFLAGS = -L. 

.PHONY : all test clean

all: client

client: client.c client.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o *.a client

