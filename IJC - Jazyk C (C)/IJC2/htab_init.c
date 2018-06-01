#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

htab *htab_init(unsigned int size){
	htab *tab = malloc(sizeof(htab));
	if (tab == NULL) return NULL;

	tab->list = malloc(sizeof(struct htab_listitem)*size);
	if (tab->list == NULL){
		free(tab);
		return NULL;
	}
	memset(tab->list, 0, sizeof(struct htab_listitem**)*size);

	tab->htab_size = size;
	tab->hash_fun_ptr = &hash_function;
	tab->n = 0;
	return tab;
}