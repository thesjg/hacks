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
#include "settings.h"


typedef struct network_buffer {
    size_t			size;
    size_t      		offset;
    size_t			used;
    uint8_t			refcnt;
    struct network_buffer 	*next;
    char			buffer[];
} *network_buffer_t;

typedef struct network_connection {
    enum {
        CONNECTION_ACCEPTED,
        CONNECTION_PARSING_COMMAND,
        CONNECTION_PARSED_COMMAND,
    } state;
    network_buffer_t		buffer;
    struct command_action	action;
} *network_connection_t;


void network_main(ecached_settings_t);
inline void network_buffer_acquire(network_buffer_t);
inline void network_buffer_release(network_buffer_t);

/* util */
int get_maxfiles(void);
socklen_t set_sockbuf_sendsize(int, size_t);
socklen_t set_sockbuf_recvsize(int, size_t);
void print_buffer(network_buffer_t);


#endif /* !_ECACHED_NETWORK_H_ */
