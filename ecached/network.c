#include "network.h"

void
network_main()
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
//                        connections[fd_new].paction = NULL;

                        EV_SET(&changes[nchanges], fd_new, EVFILT_READ, EV_ADD, 0, 0, 0);
                        ++nchanges;
                    }

                /* Existing connection */
                } else {
printf("Existing\n");
                    if (events[i].filter == EVFILT_READ) {
                        network_buffer_t *buf;
printf("Ready for READ\n");

                        if (connections[fd].buffer == NULL) {
                            /* Assume malloc(3) will be caching bufsize sized objects */
                            if ((connections[fd].buffer =
                                    (network_buffer_t *)malloc(sizeof(network_buffer_t) +
                                                               bufsize)) == NULL)
                            {
                                (void)close(fd);
                                continue;
                            }

                            connections[fd].buffer->size = bufsize;
                            connections[fd].buffer->used = 0;
                            connections[fd].buffer->buffer[0] = '\0';
                        }

                        buf = (network_buffer_t *)&connections[fd].buffer;

                        /* XXX: will go to 0 */
                        /* used not same size/type as buffer ? */
                        ssize_t rb = recv(fd, &buf->buffer[buf->used],
                                          buf->size - buf->used, 0);
                        if (rb == 0) {
printf("Ack, closing\n");
                            (void)free(connections[fd].buffer);
                            (void)close(fd);

                            /*
                             * kevent(2) does this for us on close(2)
                             *
                             * EV_SET(&changes[nchanges], events[i].ident, 0, EV_DELETE, 0, 0, 0);
                             * ++nchanges;
                             */
                        } else {
                            buf->used += rb;

                            buf->buffer[buf->used + 1] = '\0';
printf("Received: %s\n", buf->buffer);
                        }

                    /* EVFILT_WRITE */
                    } else {
printf("Ready for WRITE\n");
                    }
                }

            }
        }
    } while (false);
}
