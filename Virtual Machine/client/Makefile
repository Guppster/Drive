CC      = gcc
CFLAGS  = -g -O0 -Wall -Werror -std=gnu11
LDFLAGS = -L. 

.PHONY : all test clean

all: client.o

client.o: client.c client.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.a client

