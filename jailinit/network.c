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
 * $Id: network.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "network.h"
#include "error.h"
#include "commands.h"
#include "jailinit.h"


int active_fd;      /* eh? */



int network_start(const char *shared_ip)  {

    int listen_fd;      /* file descriptor we listen on */

    struct sockaddr_in jail_addr;     /* this is me */
    struct sockaddr_in shared_addr;   /* this is the REAL box i'm being run on (hopefully) */

    int sin_size;


  /* DEBUG */
printf("Network started, listening for connections from: %s\n", shared_ip);


    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)  {
        dk_errx("socket() failed");
    }

    jail_addr.sin_family = AF_INET;
    jail_addr.sin_port = htons(BIND_PORT);
      /* we may want the main ip of the jail here? */
    jail_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(jail_addr.sin_zero), '\0', 8);

    if (bind(listen_fd, (struct sockaddr *)&jail_addr, sizeof(struct sockaddr)) == -1)  {
        dk_errx("bind() failed");
    }


      /* Backlog of 0, we only care about our big daddy shared server  :)  */
    if (listen(listen_fd, 0) == -1)  {
        dk_errx("listen() failed");
    }


/*
 *  --- NOTE ---
 *
 * jailproc will need to bind to the primary ip on the shared machine,
 * if it doesn't then the ip we get in shared_addr is the alias the
 * current jail is using.  We can't auth using that IP, because the
 * customer would have access to jailinit at that point.
 *
 */
      /* Do the work. */
    for ( ; ; )  {
        char c_string[1000];  /* temp */
        int j, i = 0;         /* temp */


        sin_size = sizeof(struct sockaddr_in);

        active_fd = accept(listen_fd, (struct sockaddr *)&shared_addr, &sin_size);
        if (active_fd == -1)  {
            dk_errx("accept() failed");
        }

          /* DEBUG */
        printf("server: got connection from %s\n", inet_ntoa(shared_addr.sin_addr));

        if (strcmp(shared_ip, inet_ntoa(shared_addr.sin_addr)) == 0)  {

            for ( ; ; )  {
                j = 0;
                j = recv(active_fd, &c_string[j], sizeof(c_string)-i, NULL);
                if (j == 0)  {
                    break;
                }
                c_string[j] = '\0';


                if (check_command(c_string) != 0)  {
//                    printf("+'%s'\n", c_string);
                    exec_command(c_string);
                }  else  {
//                    printf("-'%s'\n", c_string);
                }

                i += j;
                c_string[0] = '\0';
            }
        }
        close(active_fd);
    }

    return 0;
}


/*
 * Check to see that a given IP exists on the host we are
 * currently running on
 */
int
check_ip_exists(const char *ip)  {
    struct ifaddrs *ifap, *my_ifap;
    struct sockaddr_in *sin;
    register unsigned int num_addrs = 0;


    if (getifaddrs(&ifap) != 0)  {
        dk_errx("getifaddrs(3) failure");
    }

    my_ifap = ifap;
    while (my_ifap->ifa_next != NULL)  {
        if (my_ifap->ifa_addr->sa_family == AF_INET)  {
            ++num_addrs;
            sin = (struct sockaddr_in *)my_ifap->ifa_addr;
        }
        my_ifap = my_ifap->ifa_next;
    }

    for (my_ifap = ifap; my_ifap->ifa_next != NULL; my_ifap = my_ifap->ifa_next)  {
        if (my_ifap->ifa_addr->sa_family == AF_INET)  {
            sin = (struct sockaddr_in *)my_ifap->ifa_addr;
            if (strcmp(ip, inet_ntoa(sin->sin_addr)) == 0)  {
                  /* Their IP exists on this box */
                return 1;
            }
        }
    }

    freeifaddrs(ifap);

    return 0;
}


/*
 * Send output to our connected socket (jailproc most likely)
 * via a printf-style interface
 */
void
transmit(const char *fmt, ...)  {
    ssize_t sent = 0;
    int bytes_to_send;
    va_list ap;
    char *sstring;
    char *my_string;

      /* Combine fmt with our given arguments */
    va_start(ap, fmt);
    bytes_to_send = vasprintf(&my_string, fmt, ap);
    va_end(ap);

    sstring = my_string;

    if (bytes_to_send != -1)  {
        for ( ; ; )  {
            sent = send(active_fd, sstring, bytes_to_send, NULL);
            if (sent == -1)  {
                dk_err("send(2) error");
//                goto get_out;
break;
            }
            if (sent == bytes_to_send)  {
//                goto get_out;
break;
            }
            bytes_to_send -= sent;
            sstring = &my_string[sent];
        }
    }

// get_out:

    free(my_string);
}


void
end_transmission(void)  {
     (void) send(active_fd, ".\r\n", 3, NULL);
}
