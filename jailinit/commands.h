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
 * $Id: commands.h,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */



/*
 * Structure with the name of a command the associated
 * function...  command should be a simple text string, "ps"
 * for instance.  Function will be called when jailinit
 * receives that text string over a socket from jailproc,
 * the function will accept no arguments, and return a pointer
 * to a character string which will be fed to jailproc.
 * Jailproc will be expecting output to be terminated by
 * \n\n, that's a pair of newlines.  :)
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>



int check_command(char *command);
void exec_command(char *command);
void unknown_command(void);



void ps(char *addl_args);
void kill_system_wrap(char *addl_args);
void restart_system_wrap(char *addl_args);
void list_commands(char *addl_args);
void terminate_process(char *addl_args);
void renice_process(char *addl_args);


#define MAX_SOCK_COMMAND_LEN 10

struct sock_commands {
    char command[MAX_SOCK_COMMAND_LEN];  /* Command name */
    char *description;                   /* Description of command */
    void (*function)(char *);            /* Pointer to command function */
};



static const struct sock_commands commands[]  =  {
    {  "ps",   "Obtain process list",  ps  },
    {  "restart",  "Restart jail",  restart_system_wrap  },
    {  "die_now",  "Kill jail",  kill_system_wrap  },
    {  "kill",  "Kill [signal] pid, terminates process",  terminate_process  },
    {  "renice", "Renice priority pid/process name, change nice-ness",  renice_process  },
    {  "help",  " ^-- Help is yours.",  list_commands  },

    {  "",   NULL }
};
