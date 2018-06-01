#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bit_array.h"
#include "eratosthenes.h"

void eratosthenes(bit_array_t pole){
	unsigned long max = (unsigned long)sqrt(ba_size(pole));

	for (unsigned long j = 2; j*2<ba_size(pole); j++){
		//printf("watch: %d\n", 2*j);
		ba_set_bit(pole, 2*j, 1);
	}
	for (unsigned long i = 3; i < max; i+=2){
		if ((ba_get_bit(pole, i)) == 0){
			//printf("prime %lu\n", i);
			//getchar();
			for (unsigned long j = i; j*i<ba_size(pole); j++){
				//printf("watch: %d\n", i*j);
				
				//printf("shift %d to [%d]<<%d\n",j*i,1 + (i*j / (sizeof((pole)) * 8)), (j*i % (sizeof((pole)) * 8)));
				//getchar();
				ba_set_bit(pole, i*j, 1);
			}//getchar();
		}
	}

	/*printf("byte1 %d\n", (unsigned char)pole[1]);
	for (unsigned long i = 2; i < ba_size(pole); i++){
		if ((ba_get_bit(pole, i)) == 0){
			printf("%lu\n", i);
		}
	}*/
}
