#	IPK1 Patrik Sztefek xsztef02

CC=gcc
CFLAGS=-std=c99 -pedantic -O2

all:
	-rm ipk-client
	-rm ipk-server
	$(CC) $(CFLAGS) client.c -o ipk-client
	$(CC) $(CFLAGS) server.c -o ipk-server

client:
	$(CC) $(CFLAGS) client.c -o ipk-client

server:
		$(CC) $(CFLAGS) server.c -o ipk-server

clean:
	-rm ipk-client
	-rm ipk-server
