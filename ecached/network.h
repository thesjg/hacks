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
} command_t;

typedef enum {
    CONNECTION_ACCEPTED,
    CONNECTION_PARSED_REQUEST,

} connection_state_t;


#endif /* _ECACHED_NETWORK_H_ */
