#ifndef BITARRAY
#define BITARRAY

#include "error.h"
//#define USE_INLINE

typedef unsigned long *bit_array_t;

#define long_bits (sizeof(unsigned long) * 8) // velikost jednoho (p)rvku bit_array_t

#define DU1_GET_BIT_(p, i) \
	( (p)[1+(i / (sizeof((p)) * 8))] & 1LU << (i % (sizeof((p)) * 8)))  ? 1 : 0

#define DU1_SET_BIT_(p, i, b) \
	(b) ? ( (p)[1+(i / (sizeof((p)) * 8))] |= 1LU<<(i%(sizeof((p)) * 8)) ) : \
		  ( (p)[1+(i / (sizeof((p)) * 8))] &= ~(1LU<<(i%(sizeof((p)) * 8))) )

//vytvori (p)ole bit_array_t
#define ba_create(name,size) bit_array_t (name) =\
	(unsigned long[((size + long_bits - 1) / (long_bits)) + 1]){(size),0,}


#ifndef USE_INLINE


//ba_size(jmeno_(p)ole)
//vrátí deklarovanou velikost (p)ole v bitech
#define ba_size(name) (name)[0]

//ba_set_bit(jmeno_(p)ole, index, výraz)
//nastaví zadaný bit v (p)oli na hodnotu zadanou výrazem
#define ba_set_bit(name,index,val) \
	(index>ba_size(name))? \
		(fatal_error("Index %ld mimo rozsah 0..%ld", (long)(index), ba_size(name)),0) : \
		DU1_SET_BIT_((name),(index),(val))



//ba_get_bit(jmeno_(p)ole, index)
//získá hodnotu zadaného bitu, vrací hodnotu 0 nebo 1
#define ba_get_bit(name,index) \
	(index>ba_size(name))? \
		(fatal_error("Index %ld mimo rozsah 0..%ld", (long)(index), ba_size(name)),1) : \
		DU1_GET_BIT_((name), (index)) 


#else
static inline unsigned long ba_size(bit_array_t name){
	return ((name)[0]);
}

static inline void ba_set_bit(bit_array_t p, unsigned long i, unsigned int b)
{
	if (i > ba_size(p))
		fatal_error("Index %ld mimo rozsah 0..%ld", (long)(i), ba_size(p));

	(b) ? ((p)[1 + (i / (sizeof((p)) * 8))] |= (unsigned long)1 << (i % (sizeof((p)) * 8))) : \
		((p)[1 + (i / (sizeof((p)) * 8))] &= ~((unsigned long)1 << (i % (sizeof((p)) * 8))));
}

static inline int ba_get_bit(bit_array_t name, unsigned long index){
	if ((index) > ba_size(name))
		fatal_error("Index %ld mimo rozsah 0..%ld", (long)(index), ba_size(name));

	return (DU1_GET_BIT_((name), (index)));
}



#endif

#endif