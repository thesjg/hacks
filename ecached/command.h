#ifndef _ECACHED_COMMAND_H_
#define _ECACHED_COMMAND_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>


struct network_connection;


#define COMMAND_MODIFY_MAX 100
typedef enum {
    COMMAND_SET		= 1,
    COMMAND_ADD		= 2,
    COMMAND_REPLACE	= 3,
    COMMAND_APPEND	= 4,
    COMMAND_PREPEND	= 5,
    COMMAND_CAS		= 6,

    COMMAND_GET		= (COMMAND_MODIFY_MAX + 1),
    COMMAND_GETS	= (COMMAND_MODIFY_MAX + 2)
} command_t;

typedef struct command_action {
    enum {
        COMMAND_PARSE_COMMAND,
        COMMAND_PARSE_KEY,
        COMMAND_PARSE_FLAGS,
        COMMAND_PARSE_EXPTIME,
        COMMAND_PARSE_SIZE,
        COMMAND_PARSE_NOREPLY,
        COMMAND_PARSE_COMPLETE
    } state;
    command_t		command;
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
} command_action_t;

#define COMMAND_PARSE_COUNT	8
#define COMMAND_PARSE_MIN_LEN	4
#define COMMAND_PARSE_SEP	' '
#define COMMAND_PARSE_TERM	"\r\n"

typedef struct commands_parse_s {
    char		*command;
    int			command_len;
    command_t		command_enum;
} commands_parse_t;


void command_init(struct network_connection *);
bool command_parse(struct network_connection *);


#endif /* !_ECACHED_COMMAND_H_ */
