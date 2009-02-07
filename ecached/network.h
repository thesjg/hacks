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
#include "protocol.h"
#include "utils.h"


typedef struct network_buffer {
    ssize_t	size;
    ssize_t	used;
    char	buffer[];
} network_buffer_t;

typedef struct network_connection {
    enum {
        CONNECTION_ACCEPTED,
        CONNECTION_PARSED_COMMAND,
    } state;
    network_buffer_t	*buffer;
    protocol_action_t	paction;
} network_connection_t;


#endif /* _ECACHED_NETWORK_H_ */
