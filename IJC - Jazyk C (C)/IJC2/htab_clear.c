#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int htab_clear(htab *t){
	struct htab_listitem *next = NULL;
	for (unsigned int i = 0; i < t->htab_size; i++){
		for (struct htab_listitem *item = t->list[i]; item != NULL; item = next){
			next = item->next;
			free(item->key);
			free(item); //smazani polozky
		}
		t->list[i] = NULL; //vynulovani
	}

	return 0;
}