#ifndef _ECACHED_HASH_H_
#define _ECACHED_HASH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/limits.h>
#include <sys/param.h>

#include "error.h"
#include "settings.h"


typedef	uint_fast8_t hash_keylen_t;
typedef uint32_t hash_t;


#define HASH_TABLES		2
#define	HASH_START_SIZE		16	/* 64k */
#define	HASH_ENTRY_OVERHEAD	(sizeof(hash_keylen_t) + sizeof(void *))
#define HASH_SEED		1

/* Grow when we exceed 100% fill */
#define HASH_GROW_CHECK(hash) \
    (((hash)->entries > (hash)->buckets) ? true : false)

typedef struct hash_table {
    struct hash_entry	*table;
    uint32_t		buckets;
    uint32_t		entries;
    uint32_t		shift;
} *hash_table_t;

typedef struct hash_entry {
    hash_keylen_t	len;
    void		*data;
    char		key[];
} *hash_entry_t;


void hash_init(ecached_settings_t);
hash_entry_t hash_search(const hash_t, const char *, const hash_keylen_t);
bool hash_insert(const hash_t, const char *, const hash_keylen_t, void *);
hash_t hash(const char *, hash_keylen_t);

#endif /* !_ECACHED_HASH_H_ */
