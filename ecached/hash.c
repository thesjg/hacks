#include "hash.h"


static hash_table_t *table[2];
static uint32_t seed = 0;
static bool resizing = false;

static hash_table_t *hash_create(uint32_t);
static bool hash_relocate_bucket(hash_table_t *, hash_table_t *, uint32_t);
static uint32_t hash(const char *, hash_keylen_t, const uint32_t);


void
hash_init(void)
{
    if ((table[0] = hash_create(HASH_START_SIZE)) == NULL)
        ecached_err(EX_SOFTWARE, "hash_create() failure");
}

hash_entry_t *
hash_search(const char *key, const hash_keylen_t len)
{
    const uint32_t hr = hash(key, len, seed);

    for (int i = 0; i < ((resizing == false) ? 1 : 2); ++i) {
        const hash_table_t *ht = table[i];
        const uint32_t offset = hr & ((1 << ht->shift)-1);
        hash_entry_t *he = (hash_entry_t *)(*((uintptr_t *)&ht->table[offset]));

        if (he != NULL) {
            while (1) {
                if (memcmp(key, he->entry, MIN(he->keylen, len)) == 0) {
                    return (he);
                } else {
                    if (he->entry[he->keylen + 1] == '\0')
                        return (NULL);

                    he = (hash_entry_t *)&he->entry[len + 1];
                    continue;
                }
            }
        } else {
            return (NULL);
        }
    }

    return (NULL);
}

bool
hash_insert(const char *key, const hash_keylen_t len, void *data)
{
    const uint32_t hr = hash(key, len, seed);
    const hash_table_t *ht = table[0];
    const uint32_t offset = hr & ((1 << ht->shift)-1);
    hash_entry_t *he;

    if ((he = (hash_entry_t *)malloc(sizeof(hash_entry_t) + len + 1)) == NULL)
        return false;

    he->keylen = len;
    he->data = data;
    memcpy((void *)he->entry, key, len);
    he->entry[len + 1] = '\0';

    *((uintptr_t *)&ht->table[offset]) = (uintptr_t)he;

    return true;
}

static hash_table_t *
hash_create(const uint32_t shiftsize)
{
    hash_table_t *ht;
    const uint32_t size = (1 << shiftsize);

    if ((ht = (hash_table_t *)malloc(sizeof(hash_table_t))) == NULL)
        return (NULL);

    if ((ht->table = (hash_entry_t *)malloc(sizeof(hash_entry_t *) * size)) == NULL)
        return (NULL);

    memset(ht->table, 0, size);

    ht->buckets = size;
    ht->shift = shiftsize;
    ht->entries = 0;

    return (ht);
}

static bool
hash_relocate_bucket(hash_table_t *source, hash_table_t *target,
                     uint32_t bucket)
{

}


/* MurmurHash2, by Austin Appleby */
static uint32_t
hash(const char *key, hash_keylen_t len, const uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const uint32_t r = 24;
    uint32_t h       = seed ^ len;
    const char *data = (const char *)(key);

    while (len >= 4) {
        uint32_t k = *(uint32_t *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (h);
}
