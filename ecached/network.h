#ifndef _ECACHED_NETWORK_H_
#define _ECACHED_NETWORK_H_

#include <fcntl.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "error.h"
#include "memory.h"
#include "utils.h"


typedef enum {
    COMMAND_SET,
    COMMAND_ADD,
    COMMAND_REPLACE,
    COMMAND_APPEND,
    COMMAND_PREPEND,
    COMMAND_CAS,

    COMMAND_GET,
    COMMAND_GETS
} network_command_t;

typedef enum {
    CONNECTION_ACCEPTED,
    CONNECTION_PARSED_COMMAND,

} network_connection_state_t;

typedef struct network_mod_action {
    .				key,
    uint32_t			flags,
    time_t			exptime, /* ? */
    .				bytes,
    boolean			noreply    
} network_mod_action_t;

typedef struct network_get_action {
    key(s)

} network_get_action_t;

typedef struct network_action {
    network_command_t		command,
    union {
        network_mod_action_t
        network_get_action_t
    }
} network_action_t;

typedef struct network_connection {
    network_connection_state_t	state,
    network_action_t		action
} network_connection_t;


#endif /* _ECACHED_NETWORK_H_ */
