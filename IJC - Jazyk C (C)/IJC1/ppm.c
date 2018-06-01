#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "bit_array.h"


struct ppm * ppm_read(const char * filename){
	//printf("in ppm read");
	struct ppm *ppm_s;
	int x, y,none;
	char none2;
	FILE *f = fopen(filename, "rb");
	if (!f)
		fatal_error("Cant open file: %s\n",filename);	

	//format
	fscanf(f, "P6 %d %d %d%c",&x,&y,&none,&none2);

	//printf("%d x:%d y:%d\n", info,x,y);

	ppm_s = malloc(sizeof(*ppm_s) + x*y*3);

	//nacteni rgb
	fread(ppm_s->data, sizeof(char), x*y*3, f);
			
	ppm_s->xsize = x;
	ppm_s->ysize = y;

	/*for (int i = 0; i < x*y*3 ; i += 3){
		printf("%d %d %d\n", (unsigned char)ppm_s->data[i], (unsigned char)ppm_s->data[i + 1], (unsigned char)ppm_s->data[i + 2]);
	getch();
	}*/

	fclose(f);
	return ppm_s;
}

int ppm_write(struct ppm *p, const char * filename){
	FILE *f = fopen(filename, "wb");

	fprintf(f, "P6 %d %d 255 ", p->xsize, p->ysize);
	fwrite(p->data, sizeof(char), 3 * p->xsize*p->ysize, f);

	fclose(f);
	return 0;
}