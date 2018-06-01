#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//vyhledání a zrušení zadané položky
int htab_remove(htab *t, const char *key){
	unsigned int hash = t->hash_fun_ptr(key, t->htab_size);
	//printf("hash: %d %s\n", hash, key);

	struct htab_listitem *prev = NULL;

	for (struct htab_listitem *item = t->list[hash]; item != NULL; item = item->next){
		if (!strcmp(key, item->key)){
			if (prev == NULL){ //first
				t->list[hash] = item->next;
				free(item->key);
				//printf("freeOK");
				free(item);
				return 0;
			}
			else{
				//TODO
				prev->next = item->next;
				free(item->key);
				free(item);
				return 0;
			}
		}
		prev = item;
	}
	return 0;
}