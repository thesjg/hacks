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
 * $Id: signal.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "signal.h"
#include "error.h"
#include "jailinit.h"



/*
 * Ignore specified signal
 */
void
sig_ign(int sig)  {
    if (signal(sig, SIG_IGN) == SIG_ERR)  {
        dk_err("Couldn't block signal %i", sig);
    }
}


/*
 *
 */
void
init_signal_handlers(void)  {

    /* We can't touch SIGKILL our SIGSTOP, so we don't try. */

      /* These signals will terminate us, we simply want to ignore them */
#ifndef DEBUG
    sig_ign(SIGINT);  /* So CTRL+C will kill us when we aren't daemonized */
#endif
    sig_ign(SIGPIPE);
    sig_ign(SIGALRM);
    sig_ign(SIGXCPU);
    sig_ign(SIGXFSZ);
    sig_ign(SIGVTALRM);
    sig_ign(SIGPROF);
    sig_ign(SIGUSR1);
    sig_ign(SIGUSR2);

      /* These signals would stop us, ignore them */
    sig_ign(SIGTSTP);
    sig_ign(SIGTTIN);
    sig_ign(SIGTTOU);

      /* This signal would kill us and create a core, ignore */
    sig_ign(SIGQUIT);

      /*
       *  These signals will terminate us and create a core image,
       *  we want them on for debugging purposes, off in production
       */
#ifndef DEBUG
    sig_ign(SIGILL);
    sig_ign(SIGTRAP);
    sig_ign(SIGABRT);
    sig_ign(SIGEMT);
    sig_ign(SIGFPE);
    sig_ign(SIGBUS);
    sig_ign(SIGSEGV);
    sig_ign(SIGSYS);
#endif


      /* We want to catch these! :) */

    if(signal(SIGHUP, restart_system) == SIG_ERR)  {
        dk_errx("Can't initialize SIGHUP handler");
    }

    if(signal(SIGTERM, kill_system) == SIG_ERR)  {
        dk_errx("Can't initialize SIGTERM handler");
    }
}
