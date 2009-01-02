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
 * $Id: error.h,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>



#define MAX_ERRMSG_LEN 1024
#define ERR_RET_CODE 0

#define ERR_OUTPUT stderr
#define WARN_OUTPUT stdout




#ifdef DEBUG
    #define dk_err(...)   _dk_errd(ERR_OUTPUT, __FILE__, __LINE__, __func__, __VA_ARGS__)
    #define dk_warn(...)  _dk_errd(WARN_OUTPUT, __FILE__, __LINE__, __func__, __VA_ARGS__)
    #define dk_errx(...)  _dk_errd(ERR_OUTPUT, __FILE__, __LINE__, __func__, __VA_ARGS__); \
                          exit(ERR_RET_CODE)
    #define dk_warnx(...) _dk_errd(WARN_OUTPUT, __FILE__, __LINE__, __func__, __VA_ARGS__); \
                          exit(ERR_RET_CODE)

    #define dk_errlog(...)  _dk_errd(NULL, __FILE__, __LINE__, __func__, __VA_ARGS__)
    #define dk_errxlog(...) _dk_errd(NULL, __FILE__, __LINE__, __func__, __VA_ARGS__); \
                            exit(ERR_RET_CODE)
#else
    #define dk_err(...)   _dk_err(ERR_OUTPUT, __VA_ARGS__)
    #define dk_warn(...)  _dk_err(WARN_OUTPUT, __VA_ARGS__)
    #define dk_errx(...)  _dk_err(ERR_OUTPUT, __VA_ARGS__); exit(ERR_RET_CODE)
    #define dk_warnx(...) _dk_err(WARN_OUTPUT, __VA_ARGS__); exit(ERR_RET_CODE)

    #define dk_errlog(...)  _dk_err(NULL, __VA_ARGS__)
    #define dk_errxlog(...) _dk_err(NULL, __VA_ARGS__); exit(ERR_RET_CODE)
#endif



void _dk_errd(FILE *output, const char *file, const int line,
              const char *func, const char *fmt, ...);
void _dk_err(FILE *output, const char *fmt, ...);
void dk_err_loginit(void);
void dk_err_nolog(void);
