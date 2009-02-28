#ifndef _ECACHED_NETWORK_H_
#define _ECACHED_NETWORK_H_

#include <fcntl.h>

#include <netinet/in.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>

#include "error.h"
#include "memory.h"
#include "command.h"


typedef struct network_buffer {
    size_t	size;
    size_t      offset;
    size_t	used;
    char	buffer[];
} network_buffer_t;

typedef struct network_connection {
    enum {
        CONNECTION_ACCEPTED,
        CONNECTION_PARSING_COMMAND,
        CONNECTION_PARSED_COMMAND,
    } state;
    network_buffer_t		*buffer;
    command_action_t		action;
} network_connection_t;


void network_main(void);

/* util */
int get_maxfiles(void);
void print_buffer(network_buffer_t *);


#endif /* _ECACHED_NETWORK_H_ */

