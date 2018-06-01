#	IPK1 Patrik Sztefek xsztef02

CC=gcc
CFLAGS=-std=c99 -pedantic -O2 -D_BSD_SOURCE

all:
	-rm ipk-dhcpstarve
	$(CC) $(CFLAGS) main.c -o ipk-dhcpstarve

clean:
	-rm ipk-dhcpstarve

run:
	./ipk-dhcpstarve -i eth0	
