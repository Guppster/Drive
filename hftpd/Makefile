CC=gcc
CFLAGS=-Wall -Werror -std=gnu11 -g -c
LFLAGS=-Wall -Werror -g

all: fig5-3 fig5-4 fig5-5 server client

fig5-3: fig5-3.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

fig5-4: fig5-4.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

fig5-5: fig5-5.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

server: server.o udp_sockets.o udp_server.o
	$(CC) $(LFLAGS) -o $@ $^

client: client.o udp_sockets.o udp_client.o
	$(CC) $(LFLAGS) -o $@ $^

fig5-3.o: fig5-3.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

fig5-4.o: fig5-4.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

fig5-5.o: fig5-5.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

server.o: server.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

client.o: client.c udp_client.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_sockets.o: udp_sockets.c udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_server.o: udp_server.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_client.o: udp_client.c udp_client.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o fig5-3 fig5-4 fig5-5 server client

