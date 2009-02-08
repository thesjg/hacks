#include "network.h"


static inline char *parse_create_token(network_buffer_t *);
static inline char *parse_find_terminator(network_buffer_t *);


void
network_command_init(network_connection_t *conn)
{
    network_action_t *action = &conn->action;

    action->state = COMMAND_PARSE_COMMAND;
}

// XXX: this will have to return something else, enumesque, to handle
// error conditions
bool
network_command_parse(network_connection_t *conn)
{
    network_action_t *action = &conn->action;
    network_buffer_t *buffer = (*conn).buffer;
    network_commands_parse_t *commands = &network_commands_parse[0];
    char *token;

start:
    if (action->state != COMMAND_PARSE_COMMAND) {
        if (action->command < NETWORK_COMMAND_MODIFY_MAX)
            goto modify;
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

modify:
    switch (action->state) {
    case COMMAND_PARSE_KEY:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        action->action.modify.key = token;
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_FLAGS;

    case COMMAND_PARSE_FLAGS:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        action->action.modify.flags = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_EXPTIME;

    case COMMAND_PARSE_EXPTIME:
        if ((token = parse_create_token(buffer)) == NULL)
            goto fail;

        action->action.modify.exptime = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_SIZE;

    case COMMAND_PARSE_SIZE:
        if ((token = parse_create_token(buffer)) == NULL) {
            if ((token = parse_find_terminator(buffer)) == NULL)
                goto fail;

            *token = '\0';
            token = &buffer->buffer[buffer->offset];
            action->action.modify.size = strtoul(token, NULL, 10);
            action->action.modify.noreply = false;
            buffer->offset += strlen(token) + strlen(COMMAND_PARSE_TERM);
            action->state = COMMAND_PARSE_COMPLETE;

            goto success;
        }

        action->action.modify.size = strtoul(token, NULL, 10);
        buffer->offset += strlen(token) + 1;
        action->state = COMMAND_PARSE_NOREPLY;

    case COMMAND_PARSE_NOREPLY:
        if ((token = parse_find_terminator(buffer)) == NULL)
            goto fail;

        *token = '\0';
        token = &buffer->buffer[buffer->offset];
        if (strncmp(token, "noreply", strlen("noreply")) == 0) {
            action->action.modify.noreply = true;
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
        buffer->offset += strlen(action->action.retrieve.key) +
                          strlen(COMMAND_PARSE_TERM);
        action->state = COMMAND_PARSE_COMPLETE;

        goto success;

    default:
        goto fail;
    }

success:

    if (action->command < NETWORK_COMMAND_MODIFY_MAX) {
        printf("K: %s\n", action->action.modify.key);
        printf("F: %d\n", action->action.modify.flags);
        printf("E: %d\n", action->action.modify.exptime);
        printf("S: %u\n", action->action.modify.size);
        printf("N: %s\n", (action->action.modify.noreply == true) ? "true" : "false");
    } else {
        printf("K: %s\n", action->action.retrieve.key);
    }

    return (true);

fail:

// XXX: check for term

    return (false);
}

static inline char *
parse_create_token(network_buffer_t *buffer)
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
parse_find_terminator(network_buffer_t *buffer)
{
    return (char *)memmem(&buffer->buffer[buffer->offset],
                          buffer->used - buffer->offset,
                          COMMAND_PARSE_TERM,
                          strlen(COMMAND_PARSE_TERM) - 1);
}

