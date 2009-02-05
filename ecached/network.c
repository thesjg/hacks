#include "network.h"


void
network_main()
{
    struct sockaddr_in sa_local;
    int fd_listen, i = 1;
    socklen_t optval, optlen = sizeof(socklen_t);

    if ((fd_listen = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        ecached_err(EX_OSERR, "socket(2) failure");

    optval = 1;
    if (setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, optlen) == -1)
        ecached_err(EX_OSERR, "setsockopt(2) failure");

    sa_local.sin_family = AF_INET;
    sa_local.sin_port = htons(11211); /* XXX: Port via getopt() */
    sa_local.sin_addr.s_addr = htonl(INADDR_ANY); /* XXX: Addr via getopt() */
    memset(&(sa_local.sin_zero), 0, sizeof(sa_local.sin_zero));

    if (bind(fd_listen, (struct sockaddr *)&sa_local,
             sizeof(struct sockaddr)) == -1)
    {
        ecached_err(EX_OSERR, "bind(2) failure");
    }

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
     * TODO: Make this a #define and implement freelist/etc.
     */
    optval = (MEMORY_ZONE_MAX >> 2);
    setsockopt(fd_listen, SOL_SOCKET, SO_RCVBUF, (void *)&optval, optlen);


    getsockopt(fd_listen, SOL_SOCKET, SO_SNDBUF, (void *)&optval, &optlen);
    printf("Send buffer size: %d\n", optval);

    getsockopt(fd_listen, SOL_SOCKET, SO_RCVBUF, (void *)&optval, &optlen);
    printf("Receive buffer size: %d\n", optval);
}
