#include "cache.h"
#include "hash.h"


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

    he = hash_search(command->action.modify.hash,
                     command->action.modify.key,
                     strlen(command->action.modify.key));
             /* XXX: move strlen calc into proto code? */


    return (NULL);
}

/*
 * Handles protocol-level commands that modify state
 */
bool
cache_modify(command_action_t command, network_buffer_t buffer)
{
    command_t cmd = command->command;;
    hash_entry_t he;

    he = hash_search(command->action.modify.hash,
                     command->action.modify.key,
                     strlen(command->action.modify.key));
             /* XXX: move strlen calc into proto code? */

    if (cmd == COMMAND_ADD) {
        return (false);
//        return (STATUS_CODE_OF_FAILURE_XXX_XXX);
    }



    return (false);
}

