#	IPK2 Patrik Szterek xsztef02

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -O2

all:
	$(CC) $(CFLAGS) main.c -o ipk-client -lcrypto

test4:
	./ipk-client 147.229.13.210
test6:
	./ipk-client 2001:67c:1220:80c::93e5:dd2

openssl:
	sudo yum install openssl
	sudo yum install openssl-devel
