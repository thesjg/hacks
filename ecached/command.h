#ifndef _ECACHED_COMMAND_H_
#define _ECACHED_COMMAND_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>

#include "hash.h"


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

typedef enum {
    COMMAND_TYPE_STORE		= 1,
    COMMAND_TYPE_RETRIEVE	= 2,
    COMMAND_TYPE_EXPIRE		= 3,
    COMMAND_TYPE_MODIFY		= 4
} command_type_t;

typedef enum {
    COMMAND_RESPONSE_VALUE		= 0001,
    COMMAND_RESPONSE_NOT_FOUND		= 0002,

    COMMAND_RESPONSE_STORED		= 1001,
    COMMAND_RESPONSE_NOT_STORED		= 1002,
    COMMAND_RESPONSE_EXISTS		= 1003,

    COMMAND_RESPONSE_DELETED		= 3001
} command_response_t;

typedef enum {
    COMMAND_ERROR		= 1,
    COMMAND_ERROR_CLIENT	= 2,
    COMMAND_ERROR_SERVER	= 3
} command_error_t;

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
    command_type_t	command_type;
    command_error_t	command_error;
    char 		*error;
    union {
        struct {
            char		*key;
            hash_keylen_t	keylen;
            hash_t		hash;

            uint32_t	flags;
            time_t	exptime;
            size_t	size;
            uint64_t	cas;
            bool	noreply;
        } store;
        struct {
            char		*key;
            hash_keylen_t	keylen;
            hash_t		hash;
        } retrieve;
    } action;
    union {
        struct {
            command_response_t	response;
        } store;
        struct {
            command_response_t	response;
            uint32_t		flags;
            size_t              size;
            uint64_t		cas;
        } retrieve;
        struct {
            command_response_t	response;
        } expire;
        struct {
            command_response_t	response;
            uint64_t		value;
        } modify;
    } response;
} *command_action_t;

#define COMMAND_PARSE_COUNT	8
#define COMMAND_PARSE_MIN_LEN	4
#define COMMAND_PARSE_SEP	' '
#define COMMAND_PARSE_TERM	"\r\n"

typedef struct commands_parse {
    char		*command;
    int			command_len;
    command_t		command_enum;
    command_type_t	command_type;
} *commands_parse_t;


void command_init(struct network_connection *);
bool command_parse(struct network_connection *);


#endif /* !_ECACHED_COMMAND_H_ */
