#include "cache.h"


void
cache_init(ecached_settings_t *settings)
{
    memory_init(settings);
    hash_init(settings);
}
