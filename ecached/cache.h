#ifndef _ECACHED_CACHE_H_
#define _ECACHED_CACHE_H_

#include <stdbool.h>

#include "error.h"
#include "memory.h"
#include "hash.h"
#include "settings.h"
#include "command.h"
#include "network.h"


#define CACHE_MAX_BUCKETS	16;

typedef hash_keylen_t cache_keylen_t;

struct network_buffer;

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
} *cache_object_t;


void cache_init(ecached_settings_t);
cache_object_t cache_retrieve(command_action_t);
bool cache_store(command_action_t, struct network_buffer *);

#endif /* !_ECACHED_CACHE_H_ */
