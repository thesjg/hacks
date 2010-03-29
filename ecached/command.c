#include "command.h"
#include "network.h"


static struct commands_parse commands_parse[] = {
    { "set",		3,	COMMAND_SET,	COMMAND_TYPE_STORE },
    { "add",		3,	COMMAND_ADD,	COMMAND_TYPE_STORE },
    { "replace",	7,	COMMAND_REPLACE,COMMAND_TYPE_STORE },
    { "append",		6,	COMMAND_APPEND,	COMMAND_TYPE_STORE },
    { "prepend",	7,	COMMAND_PREPEND,COMMAND_TYPE_STORE },
    { "cas",		3,	COMMAND_CAS,	COMMAND_TYPE_STORE },
    { "get",		3,	COMMAND_GET,	COMMAND_TYPE_RETRIEVE },
    { "gets",		4,	COMMAND_GETS,	COMMAND_TYPE_RETRIEVE },
    { NULL }
};

static inline char *parse_create_token(network_buffer_t);
static inline char *parse_find_terminator(network_buffer_t);


void
command_init(network_connection_t conn)
{
    command_action_t action = &conn->action;

    action->state = COMMAND_PARSE_COMMAND;
}

// XXX: this will have to return something else, enumesque, to handle
// error conditions
bool
command_parse(network_connection_t conn)
{
    command_action_t action = &conn->action;
    network_buffer_t buffer = conn->buffer;
    commands_parse_t commands = &commands_parse[0];
    char *token;

start:
    if (action->state != COMMAND_PARSE_COMMAND) {
        if (action->command < COMMAND_MODIFY_MAX)
            goto store;
        else
            goto retrieve;
    } else {
        if (buffer->used < COMMAND_PARSE_MIN_LEN)
            goto fail;
    }

    while (commands->command != NULL) {
        if (memcmp(&buffer->buffer[buffer->offset], commands->command,
                   MIN(COMMAND_PARSE_MIN_LEN, commands->command_len)) == 0)
        {
            buffer->offset += commands->command_len + 1; /* + whitespace */
            action->command = commands->command_enum;
            action->command_type = commands->command_type;
            break;
        }

        ++commands;
    }

    if (action->state == COMMAND_PARSE_COMMAND) {
        action->state = COMMAND_PARSE_KEY;
        goto start;
    } else {
        goto fail;
    }

store:
    switch (action->state) {
    case COMMAND_PARSE_KEY:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        action->action.store.key = token;
        action->action.store.keylen = strlen(token);
        action->action.store.hash = hash(action->action.store.key,
                                         action->action.store.keylen);
        buffer->offset += action->action.store.keylen + 1;
        action->state = COMMAND_PARSE_FLAGS;

    case COMMAND_PARSE_FLAGS:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        /* XXX: Check if really a number */
        action->action.store.flags = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_EXPTIME;

    case COMMAND_PARSE_EXPTIME:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        /* XXX: Check if really a number */
        action->action.store.exptime = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_SIZE;

/*
 * XXX
 *
 * We should one-shot SIZE and NOREPLY
 */

    case COMMAND_PARSE_SIZE:
        if ((token = parse_create_token(buffer)) == NULL) {
            if ((token = parse_find_terminator(buffer)) == NULL)
                goto fail;

            *token = '\0';
            token = &buffer->buffer[buffer->offset];
            action->action.store.size = strtoul(token, NULL, 10);
            action->action.store.noreply = false;
            buffer->offset += strlen(token) + strlen(COMMAND_PARSE_TERM);
            action->state = COMMAND_PARSE_COMPLETE;

            goto success;
        }

        if ((token = parse_find_terminator(buffer)) != NULL) {
            *token = '\0';
            token = &buffer->buffer[buffer->offset];
            action->state = COMMAND_PARSE_COMPLETE;
        }

        /* XXX: Check if really a number */
        action->action.store.size = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;

        if (action->state == COMMAND_PARSE_COMPLETE)
            goto success;

        action->state = COMMAND_PARSE_NOREPLY;

    case COMMAND_PARSE_NOREPLY:
        if ((token = parse_find_terminator(buffer)) == NULL)
            goto fail;

        *token = '\0';
        token = &buffer->buffer[buffer->offset];
        if (strncmp(token, "noreply", strlen("noreply")) == 0) {
            action->action.store.noreply = true;
            buffer->offset += strlen("noreply") + strlen(COMMAND_PARSE_TERM);
            action->state = COMMAND_PARSE_COMPLETE;

            goto success;
        }

    default:
        goto fail;
    }

retrieve:
    switch (action->state) {
    case COMMAND_PARSE_KEY:
        if ((token = parse_find_terminator(buffer)) == NULL)
            goto fail;

        *token = '\0';
        action->action.retrieve.key = &buffer->buffer[buffer->offset];
        action->action.retrieve.keylen = strlen(action->action.retrieve.key);
        action->action.retrieve.hash = hash(action->action.retrieve.key,
                                            action->action.retrieve.keylen);
        buffer->offset += action->action.retrieve.keylen +
                          strlen(COMMAND_PARSE_TERM);
        action->state = COMMAND_PARSE_COMPLETE;


        goto success;

    default:
        goto fail;
    }

success:

    if (action->command < COMMAND_MODIFY_MAX) {
        printf("K: %s\n", action->action.store.key);
        printf("F: %d\n", action->action.store.flags);
        printf("E: %d\n", action->action.store.exptime);
        printf("S: %u\n", action->action.store.size);
        printf("N: %s\n", (action->action.store.noreply == true) ? "true" : "false");
    } else {
        printf("K: %s\n", action->action.retrieve.key);
    }

    return (true);

fail:

// XXX: check for term

    return (false);
}

static inline char *
parse_create_token(network_buffer_t buffer)
{
    char *offset = (char *)memchr(&buffer->buffer[buffer->offset],
                                  COMMAND_PARSE_SEP,
                                  buffer->used - buffer->offset);
    if (offset != NULL) {
        *offset = '\0';
        return (&buffer->buffer[buffer->offset]);
    }

    return (NULL);
}

static inline char *
parse_find_terminator(network_buffer_t buffer)
{
    return (char *)memmem(&buffer->buffer[buffer->offset],
                          buffer->used - buffer->offset,
                          COMMAND_PARSE_TERM,
                          strlen(COMMAND_PARSE_TERM));
}

