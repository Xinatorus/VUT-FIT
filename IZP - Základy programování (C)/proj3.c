/** Patrik Sztefek xsztef02
* Kostra programu pro 3. projekt IZP 2015/16
*
* Jednoducha shlukova analyza: 2D nejblizsi soused.
* Single linkage
* http://is.muni.cz/th/172767/fi_b/5739129/web/web/slsrov.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
* Ladici makra. Vypnout jejich efekt lze definici makra
* NDEBUG, napr.:
*   a) pri prekladu argumentem prekladaci -DNDEBUG
*   b) v souboru (na radek pred #include <assert.h>
*      #define NDEBUG
*/
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
* Deklarace potrebnych datovych typu:
*
* TYTO DEKLARACE NEMENTE
*
*   struct objt_t - struktura objektu: identifikator a souradnice
*   struct cluster_t - shluk objektu:
*      pocet objektu ve shluku,
*      kapacita shluku (pocet objektu, pro ktere je rezervovano
*          misto v poli),
*      ukazatel na pole shluku.
*/

struct obj_t {
	int id;
	float x;
	float y;
};

struct cluster_t {
	int size;
	int capacity;
	struct obj_t *obj;
};

void print_cluster(struct cluster_t *c);
void print_clusters(struct cluster_t *carr, int narr);
/*****************************************************************
* Deklarace potrebnych funkci.
*
* PROTOTYPY FUNKCI NEMENTE
*
* IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
*
*/

/*
Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
	assert(c != NULL);
	assert(cap >= 0);

	// TODO

	struct obj_t *obj;

	if (cap == 0){ // pokud je pozadovana kapacita = 0, nealokujeme zadnou pamet 
		c->obj = NULL;
		c->capacity = 0;
	}
	else{ // jinak alokujeme potrebnou kapacitu 
		obj = (struct obj_t *)malloc(sizeof(struct obj_t)*cap);
		if (obj == NULL){
			fprintf(stderr, "Chyba alokovani pameti pro obj\n");
			c->capacity = 0;
		}
		else{
			c->obj = obj;
			c->capacity = cap;
		}
	}
	c->size = 0;
}

/*
Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
*/
void clear_cluster(struct cluster_t *c)
{
	if (c != NULL)
		free(c->obj);

	init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
*/
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
	// TUTO FUNKCI NEMENTE
	assert(c);
	assert(c->capacity >= 0);
	assert(new_cap >= 0);

	if (c->capacity >= new_cap)
		return c;

	size_t size = sizeof(struct obj_t) * new_cap;

	void *arr = realloc(c->obj, size);
	if (arr == NULL)
		return NULL;

	c->obj = arr;
	c->capacity = new_cap;
	return c;
}

/*
Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
nevejde.
*/
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	if (c->size < c->capacity){ // pokud je kapacita dostatecne velika 
		c->obj[c->size].id = obj.id;
		c->obj[c->size].x = obj.x;
		c->obj[c->size].y = obj.y;
	}
	else{ //pokud ne kapacita se zvetsi o CLUSTER_CHUNK
		resize_cluster(c, c->capacity + CLUSTER_CHUNK);
		c->obj[c->size].id = obj.id;
		c->obj[c->size].x = obj.x;
		c->obj[c->size].y = obj.y;
	}
	c->size++;
}

/*
Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
*/
void sort_cluster(struct cluster_t *c);

/*
Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
Shluk 'c2' bude nezmenen.
*/
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
	assert(c2 != NULL);

	// pokud je celkovy pocet shluku vetsi nez kapacita ciloveho clusteru
	if ((c1->size + c2->size) > c1->capacity) 
		resize_cluster(c1, c1->size + c2->size);

	//objekty c2 se vlozi do c1 a pole c1 se seradi
	for (int i = 0; i <c2->size; i++){ 
		append_cluster(c1, c2->obj[i]);
	}
	sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
(shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(idx < narr);
	assert(narr > 0);

	// TODO

	struct obj_t *tmp;

	//free((carr + idx)->obj);
	/*
	(carr + idx)->obj = (carr + narr - 1)->obj;
	(carr + idx)->size = (carr + narr - 1)->size;
	(carr + idx)->capacity = (carr + narr - 1)->capacity;*/

	//clear_cluster((carr + idx)); // smazani objektu 
	tmp = (carr + idx)->obj;
	(carr + idx)->obj = (carr + narr - 1)->obj;
	(carr + idx)->size = (carr + narr - 1)->size;
	(carr + idx)->capacity = (carr + narr - 1)->capacity;

	(carr + narr - 1)->obj = tmp;

	return narr - 1;
}

/*
Pocita Euklidovskou vzdalenost mezi dvema objekty.
*/
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1 != NULL);
	assert(o2 != NULL);

	float distance = sqrtf((o1->x - o2->x)*(o1->x - o2->x) + (o1->y - o2->y)*(o1->y - o2->y));
	return distance;
}

/*
Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
souseda.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
	assert(c1->size > 0);
	assert(c2 != NULL);
	assert(c2->size > 0);

	// zakladni hodnoty pro porovnavani
	float distance = obj_distance(c1->obj, c2->obj);
	float min = distance;

	//porovnava kazdy objekt c1 s kazdym objektem c2
	for (int i = 0; i < c1->size; i++){
		for (int j = 0; j < c2->size; j++){
			distance = obj_distance(c1->obj + i, c2->obj + j);
			if (distance < min) min = distance; // pokud se nasla nova nejmensi vzdalenost
		}
	}
	return min;
}

/*
Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	assert(narr > 0);

	// pocatecni hodnoty pro porovnavani
	float distance = cluster_distance(carr, carr + 1);
	float min = distance;
	*c1 = 0;
	*c2 = 1;

	//porovnava kazdy s kazdym 
	for (int i = 0; i < narr; i++){
		for (int j = 0; j < narr; j++){
			if (i == j)continue;// neporovnava sebe se sebou samym
			distance = cluster_distance(carr + i, carr + j);
			if (distance < min){
				min = distance;
				*c1 = i;
				*c2 = j;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
	// TUTO FUNKCI NEMENTE
	const struct obj_t *o1 = a;
	const struct obj_t *o2 = b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}

/*
Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	for (int i = 0; i < c->size; i++)
	{
		if (i) putchar(' ');
		//printf("%d>", c,c->obj);
		printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
	}
	putchar('\n');
}

/*
Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
	assert(arr != NULL);

	// TODO

	char str[30];
	int count, i;
	int id;
	float x, y;
	struct obj_t obj;

	//otevreni souboru
	FILE *f = fopen(filename, "r");
	if (f == NULL){
		*arr = NULL;
		fprintf(stderr, "File Error\n");
		return 0;
	}

	fgets(str, 30, f);
	sscanf(str, "count=%d", &count);
//TODO TESTS

	*arr = malloc(sizeof(struct cluster_t)*count); //alokovani potrebne pameti
	if (*arr == NULL){
		*arr = NULL;
		fprintf(stderr, "Chyba alokace pameti pro clustery\n");
		return 0;
	}

	//cteni souboru po radcich podle count
	for (i = 0; i < count; i++){
		fgets(str, 30, f);
		sscanf(str, "%d %f %f", &id, &x, &y);

		if (x < 0 || y < 0 || x > 1000 || y > 1000){
			fprintf(stderr, "Chyba vstupnich dat\n");
			return -1;
		}

//TODO test spravnosti
		
		obj.id = id;
		obj.x = x;
		obj.y = y;

		init_cluster(*arr + i, 1);
		append_cluster(*arr + i, obj);
	}

	fclose(f);

	return count;
}

/*
Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
	printf("Clusters:\n");
	for (int i = 0; i < narr; i++)
	{
		printf("cluster %d: ", i);
		print_cluster(&carr[i]);
	}
}

void help(){
	printf("Shlukova analyza\nParametry programu:\n filename\n count\n");
}

int main(int argc, char *argv[])
{
	struct cluster_t *clusters;

	int count,tmpCount, ca1, ca2;
	int countC = 1; // vysledny pocet clusteru

//TODO params
	if (argc == 1){ // no arg
		help();
		return 0;
	}
	else if (argc == 3){ // filename + countC
		countC = (int)strtol(argv[2], NULL, 10);
		if (countC == INT_MAX && errno == ERANGE){
			printf("Chyba arg\n");
			return -1;
		}

	}
	else if(argc > 3){ // neplatne arg programu
		printf("Neplatne argumenty\n");
		return -1;
	}

	count = load_clusters(argv[1], &clusters);
	if (count <= 0) return -1;
	tmpCount = count;

	for (; tmpCount > countC;){
		find_neighbours(clusters, tmpCount, &ca1, &ca2); // nalezeni dvou nejblizsich clusteru

		merge_clusters(clusters + ca1, clusters + ca2); // spojeni do prvniho

		tmpCount = remove_cluster(clusters, tmpCount, ca2); // smazani druheho
	}

	print_clusters(clusters, countC);


	for (int i = 0; i < count; i++){
		free(clusters[i].obj);
	}
	free(clusters);


	return 0;
}
