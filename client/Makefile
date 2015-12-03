CC      = gcc
CFLAGS=-Wall -Werror -std=gnu11 -g -c 
LFLAGS=-Wall -Werror -g

all: client

client: client.o udp_sockets.o udp_client.o
	$(CC) $(LFLAGS) -o $@ $^ -lhfs 

client.o: client.c client.h ../common/udp_client.h ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $< -lhfs 

udp_sockets.o: ../common/udp_sockets.c ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_client.o: ../common/udp_client.c ../common/udp_client.h ../common/udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o client
