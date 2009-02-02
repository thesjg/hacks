#ifndef _ECACHED_HASH_H_
#define _ECACHED_HASH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/limits.h>
#include <sys/param.h>

#include "error.h"


typedef	uint8_t hash_keylen_t;


#define	HASH_START_SIZE		16	/* (1 << HASH_START_SIZE) == 64k */
#define	HASH_ENTRY_OVERHEAD	(sizeof(hash_keylen_t) + sizeof(void *))

/* Grow on 50% fill */
#define HASH_GROW_CHECK(hash) \
    ((((hash)->entries << 1) > (hash)->buckets) ? true : false)

/* Double the current size */
#define HASH_GROW_SIZE(hash) \
    ((hash)->buckets << 1)

typedef struct hash_table {
    struct hash_entry	*table;
    uint32_t		buckets;
    uint32_t		entries;
    uint32_t		shift;
} hash_table_t;

typedef struct hash_entry {
    hash_keylen_t	keylen;
    void		*data;
    char		entry[];
} hash_entry_t;


void hash_init(void);
hash_entry_t *hash_search(const char *, const hash_keylen_t);
bool hash_insert(const char *, const hash_keylen_t, void *);


#endif /* _ECACHED_HASH_H_ */
