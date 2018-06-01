#include "htable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//zrušení celé tabulky (volá clear)
int htab_free(htab *t){
	htab_clear(t);
	free(t->list);
	free(t);

	return 0;
}