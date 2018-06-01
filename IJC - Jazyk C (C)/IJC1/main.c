#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "eratosthenes.h"
#include "bit_array.h"
#include "error.h"
#include "ppm.h"


int main(void){
	ba_create(pole, 202000000);
	
	/*printf("%dbit\n", sizeof((pole))*8);

	for (int i = 0; i < 100; i++){
		ba_set_bit(pole, i, 1);
		printf("%d  %lu %lu\n",i, pole[1], pole[2]);
	}
	*/
	//ba_set_bit(pole, 35, 1);
	//printf("%lu %lu 32th bit: %lu\n", pole[1], pole[2],ba_get_bit(pole,35));

	//getchar();

	printf("Start:\n");

	eratosthenes(pole);
	//printf("first  %lu %lu\n", pole[1], pole[2]);

	printf("Era end...<ENTER>\n");

	//for (unsigned long i = 2; i < ba_size(pole); i++){
	//	//printf("%3lu -> %lu  ", i, ba_get_bit(pole, i));
	//	if ((ba_get_bit(pole, i)) == 0){
	//		printf("%3lu -> %lu   ", i);
	//	}
	//}

	getchar();
	printf("Read ppm...\n");
	struct ppm *ppm_s = ppm_read("du1-obrazek.ppm");
	unsigned int size_ppm = ppm_s->xsize * ppm_s->ysize * 3;
	printf("Read ppm end...\n");

	/*for (int i = 0; i < size_ppm ; i += 3){
		printf("%d %d %d\n", (unsigned char)ppm_s->data[i], ppm_s->data[i + 1], ppm_s->data[i + 2]);
		getchar();
	}*/
	char message[5000]; // zprava
	int chars = 0; // pocet znaku
	char byte = 0; //byto pro uzlozeni bitu
	int nbyte = 0; // pocet bitu
	char bit = 0; 

	for (unsigned int i = 2; i < size_ppm ; i++){
		if ((ba_get_bit(pole, i)) == 0){
			//printf("%u_", i);
			bit = (ppm_s->data[i] & 1);
			//printf("b%d-%d ", bit,nbyte);
			//getch();
			
			if (nbyte > 0 && nbyte % 8 == 0){
				if (byte == '\0') break;
				message[chars++] = byte;
				

				printf("%c", byte);
				byte = 0;
				//getchar();
			}
			if (bit){	
				byte |= 1 << (nbyte % 8);
				//printf("byte> %d", byte);
			} 
			
			nbyte++;
		}
	}

	//ppm_write(ppm_s, "test.ppm");

	//printf("%d", chars);
	getchar();
	//system("pause");
	return 0;
}
