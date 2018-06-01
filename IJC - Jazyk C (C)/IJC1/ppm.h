#ifndef PPM_IJC
#define PPM_IJC

struct ppm {
	unsigned int xsize;
	unsigned int ysize;
	char data[];    // RGB bajty, celkem 3*xsize*ysize
};

struct ppm * ppm_read(const char * filename);
int ppm_write(struct ppm *p, const char * filename);


#endif