#include "network.h"


static void network_command_init(network_connection_t *);
static bool network_command_parse(network_connection_t *);
static inline char *network_command_parse_find_seperator(network_buffer_t *);
static void print_buffer(network_buffer_t *);


void
network_main(void)
{
    struct sockaddr_in sa_local;
    int fd_listen, maxfiles, i;
    uint32_t bufsize;
    socklen_t optval, optlen = sizeof(socklen_t);

    if ((fd_listen = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        ecached_err(EX_OSERR, "socket(2) failure");

    optval = 1;
    if (setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, optlen) == -1)
        ecached_err(EX_OSERR, "setsockopt(2) failure");

    /* Make socket non-blocking */
    if ((i = fcntl(fd_listen, F_GETFL)) == -1)
        i = 0;
    if (fcntl(fd_listen, F_SETFL, i | O_NONBLOCK) == -1)
        ecached_err(EX_OSERR, "fcntl(2) failure");

    /* */
    sa_local.sin_family = AF_INET;
    sa_local.sin_port = htons(11211); /* XXX: Port via getopt() */
    sa_local.sin_addr.s_addr = htonl(INADDR_ANY); /* XXX: Addr via getopt() */
    memset(&(sa_local.sin_zero), 0, sizeof(sa_local.sin_zero));

    if (bind(fd_listen, (struct sockaddr *)&sa_local, sizeof(struct sockaddr)) == -1)
        ecached_err(EX_OSERR, "bind(2) failure");

    /* Attempt to ensure listen backlog is at least 128 long */
    if ((i = listen(fd_listen, -1)) < 128)
        i = 128;

    if (listen(fd_listen, i) == -1)
        ecached_err(EX_OSERR, "listen(2) failure");

    /*
     * Attempt to set outgoing sockbuf size to the maximum size of one of
     * our buckets (or close to it).
     * With any luck this will allow us to fire off an entire bucket with
     * every call to sendfile(2), regardless of bucket size
     */
    if (getsockopt(fd_listen, SOL_SOCKET, SO_SNDBUF, (void *)&optval, &optlen) == 0)
    {
        int iter = 0;

        i = (int)optval;
        optval = MEMORY_ZONE_MAX;
        while (1) {

            setsockopt(fd_listen, SOL_SOCKET, SO_SNDBUF, (void *)&optval, optlen);
            getsockopt(fd_listen, SOL_SOCKET, SO_SNDBUF, (void *)&optval, &optlen);

            if (optval >= MEMORY_ZONE_MAX - (iter * 16384))
                break;
            else
                optval = MEMORY_ZONE_MAX - (iter * 16384);
            
            ++iter;
        }
    }

    /*
     * Attempt to set receive buffer size to something reasonable in terms
     * of memory usage but not performance degrading.
     * We will be maintaining a freelist of heap allocated memory this size
     * to do incoming scatter/gather i/o.
     *
     * TODO: Make this a #define
     */
    optval = (MEMORY_ZONE_MAX >> 2);
    setsockopt(fd_listen, SOL_SOCKET, SO_RCVBUF, (void *)&optval, optlen);
    getsockopt(fd_listen, SOL_SOCKET, SO_RCVBUF, (void *)&optval, &optlen);
    bufsize = (uint32_t)optval;

printf("BUFSIZE: %d\n", bufsize);

    maxfiles = get_maxfiles(); /* XXX: From getopt() */

    do {
        struct kevent changes[maxfiles], events[maxfiles];
        network_connection_t connections[maxfiles];
        struct sockaddr_in sa_remote;
        socklen_t remote_size = sizeof(sa_remote);
        int kq, nchanges, nevents;

        if ((kq = kqueue()) == -1)
            ecached_err(EX_OSERR, "kqueue(2) failure");

        EV_SET(&changes[0], fd_listen, EVFILT_READ, EV_ADD, 0, 0, 0);
        nchanges = 1;

        while (true) {

            nevents = kevent(kq, (struct kevent *)&changes, nchanges,
                             (struct kevent *)&events, maxfiles, NULL);
            nchanges = 0;

            for (i = 0; i < nevents; ++i) {
                int fd = events[i].ident;

                /* New connection */
                if (fd == fd_listen) {
                    int fd_new = accept(fd_listen, (struct sockaddr *)&sa_remote, &remote_size);
                    if (fd_new == -1) {
                        /* XXX: Could not accept */
                    } else {
                        connections[fd_new].state = CONNECTION_ACCEPTED;
                        connections[fd_new].buffer = NULL;

                        EV_SET(&changes[nchanges], fd_new, EVFILT_READ, EV_ADD, 0, 0, 0);
                        ++nchanges;
                    }

                /* Existing connection */
                } else {
                    if (events[i].filter == EVFILT_READ) {
                        network_buffer_t *buf;
                        network_connection_t *conn = &connections[fd];

                        if (conn->buffer == NULL) {
                            /* Assume malloc(3) will be caching bufsize sized objects */
                            if ((conn->buffer = (network_buffer_t *)malloc(sizeof(network_buffer_t)
                                              + bufsize)) == NULL)
                            {
                                (void)close(fd);
                                continue;
                            }

                            conn->state = CONNECTION_PARSING_COMMAND;
                            buf = (*conn).buffer;
                            (*buf).size = bufsize;
                            (*buf).offset = 0;
                            (*buf).used = 0;
                            (*buf).buffer[0] = '\0';

                            network_command_init(conn);
                        } else {
                            buf = (*conn).buffer;
                        }

                        /* XXX: will go to 0 */
                        ssize_t rb = recv(fd, &buf->buffer[buf->used],
                                          buf->size - buf->used, 0);
                        if (rb == 0) {
                            if (conn->buffer != NULL) {
                                (void)free(conn->buffer);
                                conn->buffer = NULL;
                            }
                            (void)close(fd);

                            /*
                             * kevent(2) does this for us on close(2)
                             *
                             * EV_SET(&changes[nchanges], events[i].ident, 0, EV_DELETE, 0, 0, 0);
                             * ++nchanges;
                             */
                        } else {
                            buf->used += rb;
                            if (connections[fd].state == CONNECTION_PARSING_COMMAND) {
buf->buffer[buf->used + 1] = '\0';

                                if (network_command_parse(conn) == true)
                                    connections[fd].state = CONNECTION_PARSED_COMMAND;
                            }
print_buffer(buf);
                        }

                    /* EVFILT_WRITE */
                    } else {
                    }
                }

            }
        }
    } while (false);
}

static void
network_command_init(network_connection_t *conn)
{
    network_action_t *action = &conn->action;

    (*action).command = COMMAND_PARSE_COMMAND;
}

static bool
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
        if ((offset = network_command_parse_find_seperator(buffer)) == NULL)
            goto fail;

        buffer->offset += (offset - (size_t)action->action.modify.key) + 1;
        action->command = COMMAND_PARSE_FLAGS;

    case COMMAND_PARSE_FLAGS:
        if ((offset = network_command_parse_find_seperator(buffer)) == NULL)
            goto fail;

        action->action.modify.flags = strtoul(&buffer->buffer[buffer->offset], NULL, 10);
        buffer->offset += (offset - (size_t)&buffer->buffer[buffer->offset]) + 1;
        action->command = COMMAND_PARSE_EXPTIME;

    case COMMAND_PARSE_EXPTIME:
        if ((offset = network_command_parse_find_seperator(buffer)) == NULL)
            goto fail;

        action->action.modify.exptime = strtoul(&buffer->buffer[buffer->offset], NULL, 10);
        buffer->offset += (offset - (size_t)&buffer->buffer[buffer->offset]) + 1;
        action->command = COMMAND_PARSE_SIZE;

    case COMMAND_PARSE_SIZE:
        if ((offset = network_command_parse_find_seperator(buffer)) == NULL) {
            char *off;
            if ((off = memchr(&buffer->buffer[buffer->offset], '\r',
                              buffer->used - buffer->offset)) == NULL)
            {
printf("did not find CR\n");
                goto fail;
            } else {
                *off = '\0';
                offset = off;
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

static void
print_buffer(network_buffer_t *buf)
{
    printf("Buffer @ 0x%x\n", buf);
    printf("\tSize: %d of %d\n", buf->used, buf->size);
    printf("\t%s\n", buf->buffer);
}
