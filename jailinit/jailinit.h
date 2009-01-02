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
 * $Id: jailinit.h,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */



#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <paths.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/jail.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>



/* system startup and shutdown scripts */
#define _PATH_RUNCOM "/etc/rc"
#define _PATH_RUNDOWN "/etc/rc.shutdown"



void start_system(void);
void restart_system(int sig);
void kill_system(int sig);
void start_jail(char *path, char *hostname, char *ip);
void daemonize(void);
