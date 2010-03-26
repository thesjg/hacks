#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "error.h"
#include "cache.h"
#include "hash.h"
#include "network.h"
#include "settings.h"


static void
usage(void)
{
    (void)fprintf(stderr, "Usage:\n%s%s%s%s%s%s%s%s",
"-p <port>         TCP port\n",
"-l <address>      Listen address\n",
"-m <megabytes>    Maximum memory usage\n",
"-c <connections>  Maximum connections\n",
"-v [<level>]      Verbosity\n",
"-P <filename>     PID file\n",
"-t                Use threads\n",
"-h                This help\n");

    exit(EX_USAGE);
}

int
main(int argc, char *argv[])
{
    int opt;
    struct ecached_settings settings;
    bool pFlag = false, lFlag = false, dFlag = false, mFlag = false,
         cFlag = false, vFlag = false, PFlag = false, tFlag = false;

    /* Setup defaults */
    settings.port = DEFAULT_PORT;
    settings.address = DEFAULT_ADDRESS;
    settings.max_mem = DEFAULT_MAX_MEM;
    settings.max_conn = DEFAULT_MAX_CONN;
    settings.verbosity = DEFAULT_VERBOSITY;
    settings.pid_file = DEFAULT_PID_FILE;
    settings.threads = DEFAULT_THREADS;

    while ((opt = getopt(argc, argv, "p:l:dm:c:v:P:t:h")) != -1) {
        switch (opt) {
        case 'p':
            /* TCP port number */
            pFlag = true;
            settings.port = (int)strtol(optarg, NULL, 10);
            /* XXX: Validate settings.port */
            break;
        case 'l':
            /* IP address */
            lFlag = true;
            if ((settings.address = strdup(optarg)) == NULL)
                ecached_err(EX_SOFTWARE, "Could not store requested listen address");
            break;
        case 'd':
            /* Daemonize */
            dFlag = true;
            break;
        case 'm':
            /* Max memory usage */
            mFlag = true;
            settings.max_mem = (uint64_t)strtoll(optarg, NULL, 10);
            break;
        case 'c':
            /* Max simultaneous connections */
            cFlag = true;
            settings.max_conn = (uint32_t)strtol(optarg, NULL, 10);
            break;
        case 'v':
            /* Verbose [level] */
            vFlag = true;
            settings.verbosity = (uint8_t)strtol(optarg, NULL, 10);
            break;
        case 'P':
            /* PID file */
            PFlag = true;
            if ((settings.pid_file = strdup(optarg)) == NULL)
                ecached_err(EX_SOFTWARE, "Could not store requested PID file path");
            break;
        case 't':
            /* Threaded */
            tFlag = true;
            settings.threads = true;
            break;
        case 'h':
            /* Help */
        default:
            usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (dFlag == true)
        daemon(0, (vFlag == true) ? 1 : 0);

    cache_init(&settings);
    network_main(&settings);

    return (EX_OK);
}
