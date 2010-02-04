/**
 *
 * This allocator creates 6 zones for storing chunks of data up to several
 * powers of 8, those between 8 and 262144. Each zone is represented as a
 * series of buckets, with each bucket able to store data sized up to the
 * stride of the zone it is in.
 *
 * Underlying each zone is an mmap(2)'ed file descriptor that has been
 * unlink(2)'ed. This arrangement allows us to grow each zone independently
 * with relative ease. Buckets are allocated in blocks of 32 and information
 * regarding the free/used state of a bucket is maintained in a heap-allocated
 * 32-bit bitmask. This makes the allocation granularity range from 256 bytes
 * to 8 megabytes.
 *
 * Each zone is initially allocated a single set of buckets, so sparse
 * allocation overhead at program startup is 9,586,944 bytes.
 *
 * The rationale behind using unlinked file-backed mmap allocations versus
 * more typical methods is that this method enables the use of zero-copy
 * mechanisms for sending data.
 */

#ifndef _ECACHED_MEMORY_H_
#define _ECACHED_MEMORY_H_

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>

#include "error.h"
#include "settings.h"


#define	MEMORY_ZONES		6		/* powers of 8 between 8 and 262144 */
#define	MEMORY_ZONE_MULT	8
#define	MEMORY_ZONE_MIN		(1U<<3)		/* 8 bytes */
#define	MEMORY_ZONE_MAX		(1U<<18)	/* 262144 bytes */
#define	MEMORY_BUCKET_SIZE	32

typedef struct memory_zone {
    void			*zone;
    int				zone_fd;
    uint32_t			quantum;
    size_t			allocated;
    uint32_t			bucket_count;
    struct memory_bucket	*buckets;
} memory_zone_t;

typedef struct memory_bucket {
    memory_zone_t	*parent_zone;
    void		*bucket;
    size_t		offset;
    uint32_t		mask;
} memory_bucket_t;


void memory_init(ecached_settings_t *);


#endif /* !_ECACHED_MEMORY_H_ */
