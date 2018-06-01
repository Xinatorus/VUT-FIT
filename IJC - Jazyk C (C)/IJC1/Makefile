CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -O2

steg-decode: steg-decode.c eratosthenes.c eratosthenes.h error.c error.h bit_array.h ppm.c ppm.h
	$(CC) $(CFLAGS) steg-decode.c eratosthenes.c error.c ppm.c -lm -o steg-decode

primes:	primes.c eratosthenes.c eratosthenes.h error.c error.h bit_array.h
	$(CC) $(CFLAGS) primes.c eratosthenes.c error.c -lm -o primes

primes-inline: primes.c eratosthenes.c eratosthenes.h error.c error.h bit_array.h
	$(CC) $(CFLAGS) -DUSE_INLINE primes.c eratosthenes.c error.c -lm -o primes-inline