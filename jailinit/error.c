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
 * $Id: error.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "error.h"



static int log_output = 0;


void
_dk_errd(FILE *output, const char *file, const int line,
         const char *func, const char *fmt, ...)  {
    char errmsg[MAX_ERRMSG_LEN];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(errmsg, MAX_ERRMSG_LEN, fmt, ap);
    va_end(ap);

    if (output == NULL  ||  log_output == 1)  {
        syslog(LOG_ERR, "%s: %s - This error was generated from file: '%s'"
                         ", on line '%i', in function '%s'\n",
                         errmsg, strerror(errno), file, line, func);
    }  else  {
        fprintf(output, "%s: %s - This error was generated from file: '%s'"
                        ", on line '%i', in function '%s'\n",
                        errmsg, strerror(errno), file, line, func);
    }
}


void
_dk_err(FILE *output, const char *fmt, ...)  {
    char errmsg[MAX_ERRMSG_LEN];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(errmsg, MAX_ERRMSG_LEN, fmt, ap);
    va_end(ap);

    if (output == NULL  ||  log_output == 1)  {
        syslog(LOG_ERR, "%s: %s\n", errmsg, strerror(errno));
    }  else  {
        fprintf(output, "%s: %s\n", errmsg, strerror(errno));
    }
}



void
dk_err_loginit(void)  {
    log_output = 1;
    openlog("jailinit", LOG_PID | LOG_NOWAIT, LOG_DAEMON);
}

void
dk_err_nolog(void)  {
    log_output = 0;
    closelog();
}
