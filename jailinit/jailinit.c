/*-
 * Copyright (c) 2001 Samuel J. Greear <dragonk@evilcode.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: jailinit.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "jailinit.h"
#include "error.h"
#include "network.h"
#include "proc.h"
#include "signal.h"



/*
 *
 * ./jailinit path hostname ip
 *
 */

int
main(int argc, char *argv[])  {

      /*
       * Check for error conditions that would
       * prevent us from working correctly
       */
    if (argc < 4)  {
          /* So we don't get debug output from dk_errx */
        fprintf(stderr, "Usage: %s path hostname ip [host ip]\n", argv[0]);
        exit(1);
    }

    if (geteuid() != 0)  {
        dk_errx("This program must be run as root");
    }

    if (getpid() == 1)  {
        dk_errx("This program will not function as a regular init(8)");
    }

/*  This breaks sometimes....
    if (check_ip_exists(argv[3]) == 0)  {
        dk_errx("The ip specified: %s, does not appear to exist on this system",
                argv[3]);
    }
*/

      /*
       * The ever-important process title change:
       * Set the title of the process (us) to the hostname
       * of the jail we are to become.
       */
    setproctitle(argv[2]);


      /* Setup signal handlers */
    init_signal_handlers();


      /* Oh yeah, throw ourselves in that cold wet concrete cell with bubba */
    start_jail(argv[1], argv[2], argv[3]);


      /* Make us go into ye-ole bg */
    daemonize();


      /* actually start the "virtual" machine. */
    start_system();


      /* Go into accept loop */
    if (argc == 5)  {
        network_start(argv[4]);
    }  else  {
        for ( ; ; )  sleep(100000);
    }

    return -1;
}


/*
 */
void
start_jail(char *path, char *hostname, char *charip)  {
    register int i;  /* Temporary usage integer variable shiz */
    int ip;          /* IP Address the jail will use (from argv) */
    struct in_addr in;
    struct jail jailconf;   /* Configuration passed to jail(2) */


      /* Lets check to see if the IP we are going to use is bound to the box */

    printf("%s - %s - %s\n", path, hostname, charip);


    memset(&jailconf, 0, sizeof(jailconf));

    jailconf.version = 0;
    jailconf.path = path;
    jailconf.hostname = hostname;

    ip = inet_aton(charip, &in);
    if (!ip)  {
        dk_errx("An IP must be specified.");
    }
    jailconf.ip_number = ntohl(in.s_addr);


      /* Jail(2) ourselves */
    i = jail(&jailconf);
    if (i == -1)  {
        dk_errx("jail(2) failed");
    }
}


void
daemonize(void)  {

#ifndef DEBUG
    if(daemon(0, 0) == -1)  {
        dk_errx("Daemonization failed");
    }

    dk_err_loginit();
#endif

}


/*
 * for HUP.
 */
void
restart_system(int sig)  {
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    kill_procs();
    start_system();

    signal(SIGHUP, restart_system);
    signal(SIGTERM, kill_system);
}


/*
 * not really used yet..
 */
void
kill_system(int sig)  {
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    kill_procs();
    exit(-1);
}


/*
 * multi user state:
 * this will involve executing RUNCOM script. typically, /etc/rc.
 */
void
start_system(void)  {
    pid_t pid;
    char *argv[3];
    int rv;

    if ((pid = fork()) == 0) {
        argv[0] = "sh";
        argv[1] = _PATH_RUNCOM;
        argv[2] = 0;

        execv(_PATH_BSHELL, argv);
        rv = 0;

        exit(rv);
    }

    wait(&rv);
    system("/usr/sbin/pwd_mkdb /etc/master.passwd");
}
