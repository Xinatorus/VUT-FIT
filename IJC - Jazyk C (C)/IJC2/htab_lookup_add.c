#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
v tabulce  t  vyhledá záznam odpovídající řetězci  key  a
- pokud jej nalezne, vrátí ukazatel na záznam
- pokud nenalezne, automaticky přidá záznam a vrátí ukazatel
Poznámka: Dobře promyslete chování této funkce k parametru key.

- Pokud htab_init nebo htab_lookup_add nemohou alokovat paměť, vrací NULL
*/
struct htab_listitem *htab_lookup_add(htab *t, const char *key){
	unsigned int hash = t->hash_fun_ptr(key, t->htab_size);

	//printf("hash: %d %s\n", hash,key);
	
	for (struct htab_listitem *item = t->list[hash]; item != NULL; item = item->next){
		if (!strcmp(key, item->key)){
			return item; //nalezl
		}
	}

	//printf("creating new %s\n",key);
	struct htab_listitem *item = malloc(sizeof(struct htab_listitem)); // nenalezl -> novy item
	if (item == NULL) return NULL;

	item->data = 0;
	item->key = malloc(strlen(key)+1); 
	if (item->key == NULL) return NULL;
	strcpy(item->key, key); //kopie retezce
	item->next = NULL; //posledni clen

	//printf("add to list %s/n",item->key);

	//add to list end
	for (struct htab_listitem *item_ = t->list[hash]; item_ != NULL; item_ = item_->next){
		if (item_->next == NULL){
			item_->next = item;
			return item;
		}
	}

	//add to list start
	//printf("is first in hash\n");
	t->list[hash] = item;

	return item;
}