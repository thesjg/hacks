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


typedef struct network_buffer {
    size_t	size;
    size_t      offset;
    size_t	used;
    char	buffer[];
} network_buffer_t;

#define NETWORK_COMMAND_MODIFY_MAX 100
typedef struct network_action {
    enum {
        COMMAND_SET 	= 1,
        COMMAND_ADD 	= 2,
        COMMAND_REPLACE = 3,
        COMMAND_APPEND 	= 4,
        COMMAND_PREPEND = 5,
        COMMAND_CAS 	= 6,

        COMMAND_GET 	= (NETWORK_COMMAND_MODIFY_MAX + 1),
        COMMAND_GETS 	= (NETWORK_COMMAND_MODIFY_MAX + 2)
    } command;
    union {
        struct {
            char	*key;
            uint32_t	hash;
            uint32_t	flags;
            time_t	exptime;
            size_t	bytes;
            bool	noreply;
        } modify;
        struct {
            char	*key;
            uint32_t	hash;
        } retrieve;
    } action;
} network_action_t;

typedef struct network_connection {
    enum {
        CONNECTION_ACCEPTED,
        CONNECTION_PARSING_COMMAND,
        CONNECTION_PARSED_COMMAND,
    } state;
    network_buffer_t		*buffer;
    network_action_t		action;
} network_connection_t;


void network_main(void);


#endif /* _ECACHED_NETWORK_H_ */

