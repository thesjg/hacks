#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "error.h"
#include "cache.h"
#include "hash.h"
#include "network.h"


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
    bool pFlag = false, lFlag = false, dFlag = false, mFlag = false,
         cFlag = false, vFlag = false, PFlag = false, tFlag = false;

    hash_entry_t *he;

    cache_init();

    hash_insert("abcd", 4, NULL);
    he = hash_search("abcd", 4);
    if (he != NULL)
        printf("%s\n", he->key);

    while ((opt = getopt(argc, argv, "p:l:dm:c:v:P:t:h")) != -1) {
        switch (opt) {
        case 'p':
            /* TCP port number */
            pFlag = true;
//            port = optarg;
            break;
        case 'l':
            /* IP address */
            lFlag = true;
            break;
        case 'd':
            /* Daemonize */
            dFlag = true;
            break;
        case 'm':
            /* Max memory usage */
            mFlag = true;
            break;
        case 'c':
            /* Max simultaneous connections */
            cFlag = true;
            break;
        case 'v':
            /* Verbose [level] */
            vFlag = true;
            break;
        case 'P':
            /* PID file */
            PFlag = true;
            break;
        case 't':
            /* Threaded */
            tFlag = true;
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

    network_main();

    return (EX_OK);
}
