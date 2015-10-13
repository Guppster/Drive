CC      = gcc
CFLAGS  = -g -O0 -Wall -Werror -std=gnu11
LDFLAGS = -L. 

.PHONY : all test clean

all: client

client: client.c fileIO.c checksum.c
	$(CC) $(CFLAGS) -o client client.c fileIO.c checksum.c -lz

clean:
	rm -f *.o *.a client