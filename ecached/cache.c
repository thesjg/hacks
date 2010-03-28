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

    he = hash_search(command->action.store.hash,
                     command->action.store.key,
                     command->action.store.keylen);
    if (he == NULL) {
        command->response.retrieve.response = COMMAND_RESPONSE_NOT_FOUND;
        ecached_warn("Attempting retrieval, but key (%s) with hash (%d) not found in hash table",
                     command->action.store.key,
                     command->action.store.hash);
        return (NULL);
    }


    return (NULL);
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

    he = hash_search(command->action.store.hash,
                     command->action.store.key,
                     command->action.store.keylen);
    if (he != NULL && cmd == COMMAND_ADD) {
        /* If key is currently in use, add fails */

        command->response.store.response = COMMAND_RESPONSE_NOT_STORED;
        return (true);
    }

    mz = memory_get_zone(command->action.store.size);

    return (false);
}

