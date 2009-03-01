#include "network.h"


/**
 * Check the maximum number of file descriptors the system will allow
 * us to have open at a given time and return this number.
 * Returns -1 on failure, or 0 if there is no limit on the fd's we can use.
 * Otherwise the maximum number of files we may have open is returned.
 * If possible, this limit will be raised.
 */
int
get_maxfiles(void)
{
    struct rlimit rl;

    if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
        if (rl.rlim_max == RLIM_INFINITY) {
            return (0);
        } else if (rl.rlim_max > rl.rlim_cur) {
            int rlim_cur = rl.rlim_cur;
            rl.rlim_cur = rl.rlim_max;
            if (setrlimit(RLIMIT_NOFILE, &rl) == 0)
                return (rl.rlim_cur);
            return (rlim_cur);
        } else {
            return (rl.rlim_cur);
        }
    }

    return (-1);
}

/**
 * Attempt to set outgoing sockbuf to the specified size. Presumably the
 * maximum size of a storage bucket.
 * With any luck this will allow us to fire off an entire bucket with
 * every call to sendfile(2), regardless of bucket size
 */
socklen_t
set_sockbuf_sendsize(int fd, size_t size)
{
    socklen_t optval, optlen = sizeof(socklen_t);

    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&optval, &optlen) == 0) {
        int iter = 0;
        optval = size;

        while (1) {

            setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&optval, optlen);
            getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&optval, &optlen);

            if (optval >= size - (iter * 16384))
                break;
            else
                optval = size - (iter * 16384);

            ++iter;
        }
    }

    return (optval);
}

/**
 * Attempt to set incoming sockbuf to the specified size.
 */
socklen_t
set_sockbuf_recvsize(int fd, size_t size)
{
    socklen_t optval, optlen = sizeof(socklen_t);

    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&optval, optlen);
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&optval, &optlen);

    return (optval);
}

void
print_buffer(network_buffer_t *buf)
{
    printf("Buffer @ 0x%x\n", (uintptr_t)buf);
    printf("\tSize: %d of %d\n", buf->used, buf->size);
    printf("\t%s\n", buf->buffer);
}
