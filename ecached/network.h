#ifndef _ECACHED_NETWORK_H_
#define _ECACHED_NETWORK_H_

#include <fcntl.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/param.h>
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
typedef enum {
    COMMAND_SET		= 1,
    COMMAND_ADD		= 2,
    COMMAND_REPLACE	= 3,
    COMMAND_APPEND	= 4,
    COMMAND_PREPEND	= 5,
    COMMAND_CAS		= 6,

    COMMAND_GET		= (NETWORK_COMMAND_MODIFY_MAX + 1),
    COMMAND_GETS	= (NETWORK_COMMAND_MODIFY_MAX + 2)
} network_command_t;

typedef struct network_action {
    enum {
        COMMAND_PARSE_COMMAND,
        COMMAND_PARSE_KEY,
        COMMAND_PARSE_FLAGS,
        COMMAND_PARSE_EXPTIME,
        COMMAND_PARSE_SIZE,
        COMMAND_PARSE_NOREPLY,
        COMMAND_PARSE_COMPLETE
    } state;
    network_command_t	command;
    union {
        struct {
            char	*key;
            uint32_t	flags;
            time_t	exptime;
            size_t	size;
            bool	noreply;

            uint32_t	hash;
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

#define NETWORK_COMMAND_PARSE_COUNT	8
#define NETWORK_COMMAND_PARSE_MIN_LEN	4
typedef struct network_commands_parse {
    char*		command;
    int			command_len;
    network_command_t	command_enum;
} network_commands_parse_t;

static network_commands_parse_t network_commands_parse[] = {
    { "set",		3,	COMMAND_SET },
    { "add",		3,	COMMAND_ADD },
    { "replace",	7,	COMMAND_REPLACE },
    { "append",		6,	COMMAND_APPEND },
    { "prepend",	7,	COMMAND_PREPEND },
    { "cas",		3,	COMMAND_CAS },
    { "get",		3,	COMMAND_GET },
    { "gets",		4,	COMMAND_GETS },
    { NULL }
};


void network_main(void);


#endif /* _ECACHED_NETWORK_H_ */

