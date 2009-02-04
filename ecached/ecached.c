#include <stdio.h>

#include "error.h"
#include "cache.h"
#include "hash.h"


int
main(int argc, char *argv[])
{
    hash_entry_t *he;

    cache_init();

    hash_insert("abcd", 4, NULL);
    he = hash_search("abcd", 4);
    if (he != NULL)
        printf("%s\n", he->key);

    return (EX_OK);
}
