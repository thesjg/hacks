#ifndef _ECACHED_CACHE_H_
#define _ECACHED_CACHE_H_

#include "error.h"
#include "memory.h"
#include "hash.h"


typedef hash_keylen_t cache_keylen_t;


typedef struct cache_object_bucket {
    int		fd;
    size_t	offset;
} cache_object_bucket_t;

typedef struct cache_object {
    size_t			size;		/* proto/internal */
    uint32_t			flags;		/* proto */
    uint_fast16_t		refcnt;		/* internal */
    uint_fast8_t		buckets;	/* internal */
    cache_object_bucket_t	data[];
} cache_object_t;


void cache_init(void);

/* These effectively map to protocol-level commands */
cache_object_t *cache_get(const char *, const cache_keylen_t);
// bool cache_set(const char *, const cache_keylen_t, <scatter/gather of incoming data in memory>, uint64_t);
bool cache_add();
bool cache_replace();
bool cache_append();
bool cache_prepend();


#endif /* _ECACHED_CACHE_H_ */
