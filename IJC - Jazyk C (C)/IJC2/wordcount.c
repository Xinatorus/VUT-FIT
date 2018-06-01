/**
	IJC DU2

	xsztef02
	Patrik Sztefek
**/
#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "io.h"

void printhTab(const char *str, unsigned int data);

int main(void){
	unsigned int size = 1000000;
	char str[127];

	struct htab_listitem *item;
	htab *tab = htab_init(size);

	if (tab == NULL){ ///kontrola
		fprintf(stderr, "Chyba htab_init\n");
		return 1;
	}

	
	//nacitani
	while (get_word(str,127,stdin)!=EOF){
		//printf("str: %s", str);
		item = htab_lookup_add(tab, str);
		if (item == NULL){
			fprintf(stderr, "Chyba lookup\n");
			htab_free(tab);
			return 1;
		}
		item->data++;
	}
	
	htab_foreach(tab, &printhTab);

	htab_free(tab);

	system("pause");
	return 0;
}

void printhTab(const char *str, unsigned int data){
	printf("%s\t%d\n", str, data);
}

