#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "io.h"


int get_word(char *s, int max, FILE *f){
	char c;
	int i=0;
	//int err;
	c = (char)getc(f);

	while (isspace(c) && c != EOF) // mezery pred slovem
	{
		c = (char)getc(f);
	}

	while (!isspace(c)&& c!=EOF){
		//printf("%c", c);
		if (i < (max - 1)){
			s[i++] = c;
			s[i] = '\0';
		}		
		c = (char)getc(f);
	}
	//printf("\n");
	if (c==EOF) return EOF;
	return i;
}