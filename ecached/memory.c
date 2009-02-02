#include "memory.h"
#include "error.h"


static memory_zone_t *zones[MEMORY_ZONES];

static memory_zone_t *memory_zone_create(size_t);
static bool memory_bucket_allocate(memory_zone_t *);
static inline size_t pow8_ceil(size_t);


void
memory_init(void)
{
    int i;
    size_t size = MEMORY_ZONE_MIN;

    for (i = 0; i < MEMORY_ZONES; ++i) {
        zones[i] = memory_zone_create(size);
        size *= MEMORY_ZONE_MULT;
    }
}

static memory_zone_t *
memory_zone_create(size_t quantum)
{
    char tfile[] = "/tmp/ecached.XXXXXXXX";
    memory_zone_t *zone;
    int fd;

    if ((quantum % 8) != 0)
        ecached_err(EX_SOFTWARE, "Memory zone quantum must be divisible by 8");

    if ((zone = (memory_zone_t *)malloc(sizeof(memory_zone_t))) == NULL)
        ecached_err(EX_OSERR, "malloc(3) failure");

    if ((fd = mkstemp(tfile)) == -1)
        ecached_err(EX_OSERR, "mkstemp(3) failure");

    if (unlink(tfile) == -1)
        ecached_err(EX_OSERR, "unlink(2) failure");

    zone->zone_fd      = fd;
    zone->zone         = NULL;
    zone->quantum      = quantum;
    zone->allocated    = 0;
    zone->bucket_count = 0;
    zone->buckets      = NULL;

    /* Setup an initial bucket for every zone */
    memory_bucket_allocate(zone);

    return (zone);
}

static bool
memory_bucket_allocate(memory_zone_t *zone)
{
    memory_bucket_t *bucket, *buckets;
    size_t size = zone->allocated + (zone->quantum * MEMORY_BUCKET_SIZE);

    if ((bucket = (memory_bucket_t *)malloc(sizeof(memory_bucket_t))) == NULL)
        ecached_err(EX_OSERR, "malloc(3) failure");

    if (ftruncate(zone->zone_fd, size) == -1)
        ecached_err(EX_OSERR, "ftruncate(2) failure");

    if (zone->zone != NULL)
        if (munmap(zone->zone, zone->allocated) == -1)
            ecached_err(EX_OSERR, "munmap(2) failure");

    if ((zone->zone = mmap((void*)0, size, PROT_READ|PROT_WRITE,
                           MAP_NOCORE|MAP_PRIVATE|MAP_NOSYNC,
                           zone->zone_fd, 0)) == NULL)
    {
        ecached_err(EX_OSERR, "mmap(2) failure");
    }

    bucket->parent_zone = zone;
    bucket->bucket      = ((int8_t *)zone->zone) + zone->allocated;
    bucket->offset      = zone->allocated;
    bucket->mask        = 0xffffffff;

    zone->bucket_count++;
    zone->allocated = size;

    if ((buckets = realloc(zone->buckets,
                           sizeof(bucket)*zone->bucket_count)) != NULL)
    {
        zone->buckets = buckets;
        zone->buckets[zone->bucket_count] = *bucket;
    } else {
        return false;
    }

    return true;
}

static inline size_t
pow8_ceil(size_t val)
{
    --val;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    ++val;

    return (val);
}
