/**
	IJC DU2

	xsztef02
	Patrik Sztefek
**/
#ifndef HTABLE
#define HTABLE


typedef struct htab_listitem{
	char *key; //ukazatel na retezec
	unsigned int data; //pocet vyskytu
	struct htab_listitem *next; //ukazatel na dalsi prvek
}htab_listitem_t;

typedef struct{
	unsigned int htab_size; // velikost pole
	unsigned int(*hash_fun_ptr)(const char*, unsigned int); // implicitnì obsahuje &hash_function
	unsigned int n; // aktuální poèet záznamù
	struct htab_listitem **list;
}htab;


unsigned int hash_function(const char *str, unsigned int htab_size);

htab *htab_init(unsigned int size);

htab *htab_init2(unsigned int size, unsigned int(*hash_function)(const char*, unsigned int));

struct htab_listitem *htab_lookup_add(htab *t, const char *key);

void htab_foreach(htab *t, void(*func)(const char*, unsigned int));

int htab_remove(htab *t,const char *key);

int htab_clear(htab *t);

int htab_free(htab *t);

#endif