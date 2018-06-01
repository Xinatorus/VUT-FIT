/*
 * Patrik Sztefek
 * xsztef02
 * VUT-FIT 2015
 */



/**
 * Struktura objektu 
 */
struct obj_t {
	/// Identifikacni cislo
	int id;

	/// Souradnice x
	float x;

	/// Souradnice y
	float y;
};
/**
 * Struktura clusteru 
 */
struct cluster_t {
	/// Pocet objektu v clusteru
	int size;

	/// Kapacita clusteru, maximalni pocet obj
	int capacity;

	/// Pole objektu
	struct obj_t *obj;
};

/**
 * @defgroup cluster operace nad clusterem
 * @addtogroup cluster
 * @{
 */

/**
 * Inicializuje cluster. Alokuje pamet pro zadanou kapacitu objetku.
 *
 * @param      c     Ukazatel na cluster 
 * @param[in]  cap   Kapacita clusteru 
 * 
 * @pre      c neni NULL    
 *	
 * @post     c ma kapacitu cap
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Odstrani objekty v clusteru 
 *
 * @param      c      Ukazatel na cluster 
 * 
 * @pre      c neni NULL  
 * @pre      cap neni zporne  
 *	
 * @post     c neobsahuje zadne obj
 */
void clear_cluster(struct cluster_t *c);


/**
 * Doporucena hodnota pro resize cluster
 */
extern const int CLUSTER_CHUNK;

/**
 * Rozsireni kapacity clusteru. 
 *
 * @param      c         Ukazatel na cluster
 * @param[in]  new_cap   Nova velikost kapacity
 *
 * @return      Vrací ukazatel na cluster
 * 
 * @pre      c neni NULL    
 * @pre      new_cap neni zaporne
 *	
 * @post     c ma novou velikost new_cap
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Vlozi obhekt na konec clusteru. Rozsiri cluster pokud je kapacita mala.
 *
 * @param      c      Ukazatel na cluster  
 * @param[in]  obj    Novy objekt 
 * 
 * @pre      c, neni NULL   
 * @pre      obj obsahuje data 
 *	
 * @post     c obsahuje nove objekt obj
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Prida objekty c2 do clusteru c2.
 *
 * @param      c1     Ukazatel na cilovy cluster 
 * @param      c2     Ukazatel na cluster 
 * 
 * @pre      c1, c2 neni NULL    
 *	
 * @post     c1 obsahuje nove prvky z c2
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);



/**
 * Vzdalenost mezi dvemi clustery 
 *
 * @param      c1     Ukazatel na prvni cluster 
 * @param      c2     Ukazatel na druhy cluster 	
 *
 * @return      Vdalenost clusteru 
 * 
 * @pre      c1, c2 neni NULL    
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);


/**
 * Serazeni objektu v clusteru  
 *
 * @param      c      Ukazatel na cluster 
 * 
 * @pre      c1, c2 neni NULL    
 */
void sort_cluster(struct cluster_t *c);

/**
 * Vypise jeden cluster 
 *
 * @param      c      Ukazatel na cluseter 
 * 
 * @pre      c1, c2 neni NULL 
 */
void print_cluster(struct cluster_t *c);

///@}



/**
 * @defgroup clusters operace s polem clusteru
 * @addtogroup clusters
 * @{
 */

/**
 * Odstrani cluster z pole clusteru. 
 *
 * @param      carr   Ukazatel na pole clusteru 
 * @param[in]  narr   Pocet clusteru 
 * @param[in]  idx    Index clusteru, ktery chceme 
 *
 * @return      Novy pocet clusteru 
 * 
 * @pre      carr,narr neni NULL    
 *	
 * @post     prvek z pole carrna indexu idx byl smazan
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Nalezeni dvou nejblizsich clusteru. 
 *
 * @param      carr   Ukazatel na pole clusteru 
 * @param[in]  narr   Pocet clusteru 
 * @param	  c1     Index prvniho clusteru 
 * @param     c2     Index druheho clusteru 
 * 
 * @pre      c1, c2 neni NULL    
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Vypise vsechny clustery 
 *
 * @param      carr   Pole clusteru 
 * @param[in]  narr   Pocet clusteru 
 * 
 * @pre      carr, narr neni NULL 
 * 
 * 
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * Nacte data o obbjektech ze souboru. Vytvori pole clusteru. 
 *
 * @param      filename  Nazev souboru 
 * @param      arr       Ukazatel na adresu pole clusteru 
 *
 * @return      Pocet nactenych clusteru. NULL pokud doslo k chybe. 
 * 
 * @pre      filename, arr neni NULL 
 */
int load_clusters(char *filename, struct cluster_t **arr);

///@}



/**
 *@defgroup object prace s objekty
 *@addtogroup object
 * Vypocitani vzdalekosti mezi dvemi objekty. 
 *
 * @param      o1     Prvni objekt 
 * @param      o2     Druhy objekt 
 *
 * @return      Vzdalenost objektu 
 * 
 * @pre      o1, o2 neni NULL    
 *	
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);
