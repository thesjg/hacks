#include "network.h"


static inline char *network_command_parse_find_seperator(network_buffer_t *);


void
network_command_init(network_connection_t *conn)
{
    network_action_t *action = &conn->action;

    (*action).command = COMMAND_PARSE_COMMAND;
}

bool
network_command_parse(network_connection_t *conn)
{
    network_action_t *action = &conn->action;
    network_buffer_t *buffer = (*conn).buffer;
    network_commands_parse_t *commands = &network_commands_parse;
    size_t offset;

start:
    if (action->command != COMMAND_PARSE_COMMAND) {
        if (action->command < NETWORK_COMMAND_MODIFY_MAX)
            goto modify;
        else
            goto retrieve;
    } else {
        if (buffer->used < NETWORK_COMMAND_PARSE_MIN_LEN)
            goto fail;
    }

    while (commands->command != NULL) {
        if (memcmp(&buffer->buffer[buffer->offset], commands->command,
                   MIN(NETWORK_COMMAND_PARSE_MIN_LEN, commands->command_len)) == 0)
        {
            buffer->offset += commands->command_len + 1; /* + whitespace */
printf("CMD NEW OFFSET: %d\n", buffer->offset);
            action->command = commands->command_enum;
            break;
        }

        ++commands;
    }

    if (action->command != COMMAND_PARSE_COMMAND) {
        action->state = COMMAND_PARSE_KEY;
        action->action.modify.key = &buffer->buffer[buffer->offset];
        goto start;
    } else {
        goto fail;
    }

modify:
    switch (action->command) {
    case COMMAND_PARSE_KEY:
        if ((offset = (size_t)network_command_parse_find_seperator(buffer)) == 0)
            goto fail;

        buffer->offset += (offset - (size_t)action->action.modify.key) + 1;
        action->command = COMMAND_PARSE_FLAGS;

    case COMMAND_PARSE_FLAGS:
        if ((offset = (size_t)network_command_parse_find_seperator(buffer)) == 0)
            goto fail;

        action->action.modify.flags = strtoul(&buffer->buffer[buffer->offset], NULL, 10);
        buffer->offset += (offset - (size_t)&buffer->buffer[buffer->offset]) + 1;
        action->command = COMMAND_PARSE_EXPTIME;

    case COMMAND_PARSE_EXPTIME:
        if ((offset = (size_t)network_command_parse_find_seperator(buffer)) == 0)
            goto fail;

        action->action.modify.exptime = strtoul(&buffer->buffer[buffer->offset], NULL, 10);
        buffer->offset += (offset - (size_t)&buffer->buffer[buffer->offset]) + 1;
        action->command = COMMAND_PARSE_SIZE;

    case COMMAND_PARSE_SIZE:
        if ((offset = (size_t)network_command_parse_find_seperator(buffer)) == 0) {
            char *off;
            if ((off = memchr(&buffer->buffer[buffer->offset], '\r',
                              buffer->used - buffer->offset)) == NULL)
            {
printf("did not find CR\n");
                goto fail;
            } else {
                *off = '\0';
                offset = (size_t)off;
                buffer->offset += 1;
                action->command = COMMAND_PARSE_COMPLETE;
            }
        }

        action->action.modify.size = strtoul(&buffer->buffer[buffer->offset], NULL, 10);
        buffer->offset += (offset - (size_t)&buffer->buffer[buffer->offset]) + 1;

        if (action->command == COMMAND_PARSE_COMPLETE)
            goto success;
        else
            action->command = COMMAND_PARSE_NOREPLY;

    case COMMAND_PARSE_NOREPLY:

        if (buffer->used > (buffer->offset + 2)) {
            if (memcmp(&buffer->buffer[buffer->offset], "\r\n", 2) == 0) {
                action->action.modify.noreply = false;
                buffer->offset += 2;
                action->command = COMMAND_PARSE_COMPLETE;
                goto success;
            } 
        } else if (buffer->used > (buffer->offset + 9)) {
            if (memcmp(&buffer->buffer[buffer->offset], "noreply\r\n", 9) == 0) {
                action->action.modify.noreply = true;
                buffer->offset += 9;
                action->command = COMMAND_PARSE_COMPLETE;
                goto success;
            }
        }
    }

    goto fail;

retrieve:
    switch (action->command) {
    case COMMAND_PARSE_KEY:
    case COMMAND_PARSE_COMPLETE:
        goto success;
        break;
    }

    goto fail;


success:

printf("K: %s\n", action->action.modify.key);
printf("F: %d\n", action->action.modify.flags);
printf("E: %d\n", action->action.modify.exptime);
printf("S: %u\n", action->action.modify.size);
printf("N: %s\n", (action->action.modify.noreply) ? "true" : "false");

    return (true);

fail:

    return (false);
}

static inline char *
network_command_parse_find_seperator(network_buffer_t *buf)
{
    char *off = memchr(&buf->buffer[buf->offset], ' ', buf->used - buf->offset);
    if (off != NULL)
        *off = '\0';

    return (off);
}

