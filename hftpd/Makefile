CC=gcc
CFLAGS=-Wall -Werror -std=gnu11 -g -c 
LFLAGS=-Wall -Werror -g

all: server 

server: server.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^ -lhdb 

server.o: server.c ../common/udp_server.h ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_sockets.o: ../common/udp_sockets.c ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_server.o: ../common/udp_server.c ../common/udp_server.h ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o server

