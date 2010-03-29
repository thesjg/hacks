#include "cache.h"
#include "hash.h"
#include "command.h"
#include "memory.h"


void
cache_init(ecached_settings_t settings)
{
    memory_init(settings);
    hash_init(settings);
}

cache_object_t
cache_retrieve(command_action_t command)
{
    hash_entry_t he;
    cache_object_t co;

    he = hash_search(command->action.retrieve.hash,
                     command->action.retrieve.key,
                     command->action.retrieve.keylen);
    if (he == NULL) {
        command->response.retrieve.response = COMMAND_RESPONSE_NOT_FOUND;
        return (NULL);
    }

    co = (cache_object_t)he->data;

    command->response.retrieve.response = COMMAND_RESPONSE_VALUE;
    command->response.retrieve.flags = co->flags;
    command->response.retrieve.size = co->size;
    command->response.retrieve.cas = 88; /* XXX */

    return (co);
}

/*
 * Handles protocol-level commands that store data
 *
 * Returns false to signify a protocol-level error condition, true otherwise
 */
bool
cache_store(command_action_t command, network_buffer_t buffer)
{
    command_t cmd = command->command;;
    hash_entry_t he;
    memory_zone_t mz;
    memory_bucket_t mb;
    cache_object_t co;
    int offset, i;

    he = hash_search(command->action.store.hash,
                     command->action.store.key,
                     command->action.store.keylen);
    if (he != NULL && cmd == COMMAND_ADD) {
        /* If key is currently in use, add fails */

        command->response.store.response = COMMAND_RESPONSE_NOT_STORED;
        return (true);
    }

    if ((mz = memory_get_zone(command->action.store.size)) == NULL)
        return (false);    

    for (i = 0; i < mz->bucket_count; ++i) {
        mb = &mz->buckets[i];

        /* No free entries, continue to next bucket */
        if (mb->mask == 0)
            continue;

        // 1 bucket
        if ((co = (cache_object_t)malloc(sizeof(*co) + sizeof(cache_object_bucket_t))) == NULL)
            return (false);

        offset = ffs(mb->mask);
        mb->mask &= ~(1 << offset);

        memcpy(&buffer->buffer[buffer->offset],
               &mb->bucket + offset * mz->quantum,
               command->action.store.size);

        co->size = command->action.store.size;
        co->flags = command->action.store.flags;
        co->refcnt = 0;
        co->buckets = 1;
        co->data[0].fd = mz->zone_fd;
        co->data[0].offset = mb->offset + (offset * mz->quantum);

        if (hash_insert(command->action.store.hash,
                        command->action.store.key,
                        command->action.store.keylen,
                        co)) {

            command->response.store.response = COMMAND_RESPONSE_STORED;
            return (true);
        }

        command->response.store.response = COMMAND_RESPONSE_NOT_STORED;

//        free(
//        unset bit
    }

    return (false);
}

