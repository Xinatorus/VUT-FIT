#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "eratosthenes.h"
#include "bit_array.h"
#include "error.h"
#include "ppm.h"


int main(int argc,char *argv[]){
	ba_create(pole, 10000);
	eratosthenes(pole);
	
	if (argc < 2){
		fatal_error("Arg error\nUse ./steg-decode ppm_filename\n");
	}

	struct ppm *ppm_s = ppm_read(argv[1]);
	unsigned int size_ppm = ppm_s->xsize * ppm_s->ysize * 3;
	
	char message[5000]; // zprava
	int chars = 0; // pocet znaku
	char byte = 0; //byto pro uzlozeni bitu
	int nbyte = 0; // pocet bitu
	char bit = 0;

	for (unsigned int i = 2; i < size_ppm; i++){
		if ((ba_get_bit(pole, i)) == 0){
			bit = (ppm_s->data[i] & 1);
			if (nbyte > 0 && nbyte % 8 == 0){
				if (byte == '\0') break;
				message[chars++] = byte;
				byte = 0;
			}
			if (bit){
				byte |= 1 << (nbyte % 8);
			}
			nbyte++;
		}
	}

	printf("%s\n", message);

	return 0;
}
