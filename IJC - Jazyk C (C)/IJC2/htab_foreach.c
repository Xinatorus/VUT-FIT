#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//volání funkce func pro každý prvek
void htab_foreach(htab *t, void(*func)(const char*, unsigned int)){
	for (unsigned int i = 0; i < t->htab_size; i++)
		for (struct htab_listitem *item = t->list[i]; item != NULL; item = item->next)
			func(item->key, item->data);
}