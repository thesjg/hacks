#include "utils.h"


/*
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

