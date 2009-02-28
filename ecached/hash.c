#include "hash.h"


static hash_table_t *table[HASH_TABLES];
static int active_tables = 1;

static hash_table_t *hash_create(uint32_t);
static void hash_relocate_bucket(void);


void
hash_init(void)
{
    if ((table[0] = hash_create(HASH_START_SIZE)) == NULL)
        ecached_err(EX_SOFTWARE, "hash_create() failure");
}

hash_entry_t *
hash_search(const hash_t hash, const char *key, const hash_keylen_t len)
{
    for (int i = 0; i < active_tables; ++i) {
        const hash_table_t *ht = table[i];
        const uint32_t offset = hash & ((1 << ht->shift)-1);
        hash_entry_t *he = (hash_entry_t *)(*((uintptr_t *)&ht->table[offset]));

        if (he != NULL) {
            while (1) {
                if (memcmp(key, he->key, MIN(he->len, len)) == 0)
                    return (he);
                else
                    if ((he = (hash_entry_t *)&he->key[he->len + 1]) == NULL)
                        return (NULL);

                /* XXX: Where to put this? */
                if (active_tables > 1)
                    hash_relocate_bucket();
            }
        } else {
            return (NULL);
        }
    }

    /* not reached */
    return (NULL);
}

bool
hash_insert(const hash_t hash, const char *key, const hash_keylen_t len, void *data)
{
    const hash_table_t *ht = table[0];
    const uint32_t offset = hash & ((1 << ht->shift)-1);
    hash_entry_t *he;

    if ((he = (hash_entry_t *)malloc(sizeof(hash_entry_t) + len + 1)) == NULL)
        return false;

    he->len = len;
    he->data = data;
    memcpy((void *)he->key, key, len);
    he->key[len + 1] = '\0';

    *((uintptr_t *)&ht->table[offset]) = (uintptr_t)he;

    if (active_tables == 1 && HASH_GROW_CHECK(ht) == true) {
        hash_table_t *nht = hash_create(table[0]->shift + 1);
        if (nht != NULL) {
            table[1] = table[0];
            table[0] = nht;
            ++active_tables;
        }
    }

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

void
hash_relocate_bucket(void)
{
    static uint32_t bucket = 0;
    hash_table_t *ht = table[0] + bucket;

    while (ht == NULL) {
        if (bucket >= ht->buckets) {
            free(table[1]);
            --active_tables;
            bucket = 0;
        }

        ++ht;
        ++bucket;
    }

    // walk buckets and move
}


/* MurmurHash2, by Austin Appleby */
hash_t
hash(const char *key, hash_keylen_t len)
{
    const uint32_t m = 0x5bd1e995;
    const uint32_t r = 24;
    const uint32_t seed = HASH_SEED;
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
