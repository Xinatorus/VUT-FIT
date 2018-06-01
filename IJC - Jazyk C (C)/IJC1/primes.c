#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "bit_array.h"
#include "eratosthenes.h"
#include "error.h"

int main(int argc, char *argv[]){
	//printf("primes\n");
	ba_create(pole, 202000000);
	eratosthenes(pole);
	unsigned long primes[11] = { 0, };
	//printf("primes end");
	//getchar();
	/*printf("size :	%lu\n", ba_size(pole));
	printf("set 3.th bit to 1\n");
	ba_set_bit(pole, 3, 1);
	printf("lu: %lu\n", pole[1]);

	printf("bit: %lu\n", ba_get_bit(pole, 3));

*/
	for (unsigned long i = 2; i < ba_size(pole); i++){
		if ((ba_get_bit(pole, i)) == 0){
			// je prvocislo
			primes[10] = i;
			for (int i = 0; i < 10; i++){
				primes[i] = primes[i + 1];
			}
		}
	}

	for (int i = 0; i < 10; i++){
		printf("%lu\n", primes[i]);
	}
	//getchar();
	return 0;
}
