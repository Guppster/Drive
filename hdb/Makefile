CC      = gcc
CFLAGS  = -g -O0 -Wall -Werror -std=gnu11
LDFLAGS = -L. 

.PHONY : all test clean

all: libhdb.a testlibhdb

libhdb.a: hdb.o
	ar rcs libhdb.a hdb.o

hdb.o: hdb.c hdb.h
	$(CC) $(CFLAGS) -o $@ -c $<

testlibhdb: libhdb.a testlibhdb.o
	$(CC) -o $@ $^ $(LDFLAGS) -lhdb -lcheck -lhiredis -lm -lpthread -lrt

testlibhdb.o: testlibhdb.c hdb.h
	$(CC) $(CFLAGS) -o $@ -c $<

test: testlibhdb
	./testlibhdb

clean:
	rm -f *.o *.a testlibhdb

