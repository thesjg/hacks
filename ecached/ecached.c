#include <stdio.h>

#include "error.h"
#include "memory.h"
#include "hash.h"


int
main(int argc, char *argv[])
{
    hash_init();
    memory_init();

    hash_insert("abcd", 4, NULL);
    hash_search("abcd", 4);

    return (EX_OK);
}
