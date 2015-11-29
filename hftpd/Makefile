CC=gcc
CFLAGS=-Wall -Werror -g -c
LFLAGS=-Wall -Werror -g

all: client1 client2 client3 server1 server2 server3 endian poll

client%: client%.o udp_sockets.o udp_client.o
	$(CC) $(LFLAGS) -o $@ $^

server%: server%.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

endian: endian.c
	$(CC) $(LFLAGS) -o $@ $^

poll: poll.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

client%.o: client%.c udp_client.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

server%.o: server%.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

poll.o: poll.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_sockets.o: udp_sockets.c udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_server.o: udp_server.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_client.o: udp_client.c udp_client.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o client[1-3] server[1-3] endian poll

