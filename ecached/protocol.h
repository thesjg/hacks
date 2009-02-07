#ifndef _ECACHED_PROTOCOL_H_
#define _ECACHED_PROTOCOL_H_

#include <stdbool.h>


typedef struct network_mod_action {
    char	*key;
    uint32_t	hash;
    uint32_t	flags;
    time_t	exptime;
    size_t	bytes;
    bool	noreply;
} protocol_action_mod_t;

typedef struct network_get_action {
    char	*key;
    uint32_t	hash;
} protocol_action_get_t;

typedef struct network_action {
    enum {
        COMMAND_SET,
        COMMAND_ADD,
        COMMAND_REPLACE,
        COMMAND_APPEND,
        COMMAND_PREPEND,
        COMMAND_CAS,

        COMMAND_GET,
        COMMAND_GETS
    } command;
    union {
        protocol_action_mod_t	mod;
        protocol_action_get_t	get;
    } action;
} protocol_action_t;


#endif /* _ECACHED_PROTOCOL_H_ */
