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
 * $Id: commands.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "commands.h"
#include "jailinit.h"
#include "network.h"
#include "error.h"
#include "proc.h"


/*
 * Get process list and send to person connected to our socket
 */
void
ps(char *addl_args)  {
    register int i;
    int num_procs;
    struct process_info *sp = NULL;


    if (addl_args != NULL)  {
        transmit(" ** %s\n", addl_args);
    }

    num_procs = get_proc_info(&sp);

    for (i = 0; i < num_procs; ++i)  {
        transmit("%d: %s\r\n", sp[i].pid, sp[i].name);
    }

    free_get_proc_info(sp);

    end_transmission();

    return;
}


/*
 * Check to see if we have a complete command ready..
 */
int
check_command(char *command)  {
    register int i;
    char *sep = " ";
    char *cmd_name;
    char cmd_copy[MAX_SOCK_COMMAND_LEN];


    if (command[0] == '\r')  {
        unknown_command();
        return 0;
    }


      /* Commands to use are terminated via \r\n, if it exists we have
         a command so we can strip them */
    if (strstr(command, "\r\n") != NULL)  {
        command[strlen(command)-2] = '\0';

        for (i = 0; ; ++i)  {
            if (commands[i].function == NULL)  {
                break;
            }  else  {
                strncpy(cmd_copy, command, MAX_SOCK_COMMAND_LEN);
                cmd_name = strtok(cmd_copy, sep);

                  /* That command exists and can be executed (everything up to
                     first " " in char *command) */
                if (strncmp(cmd_name, commands[i].command, MAX_SOCK_COMMAND_LEN) == 0)  {
                    return 1;
                }
            }
        }
    }

    unknown_command();
    return 0;
}


/*
 * Actually execute the command, and possibly send output
 * to our connected socket.  Check for additional arguments
 * and send those to proper output function as well.
 */
void
exec_command(char *command)  {
    register int i;
    char *sep = " ";
    char *cmd_name;
    char *addl_args;
    char cmd_copy[strlen(command)];

    strcpy(cmd_copy, command);
    cmd_name = strtok(cmd_copy, sep);
    for (i = 0; ; ++i)  {
        if (commands[i].function == NULL)  {
            break;
        }  else  {
            if (strcmp(cmd_name, commands[i].command) == 0)  {
                if (strlen(command) > strlen(cmd_name))  {
                    addl_args = &command[strlen(cmd_name)+1];
                    commands[i].function(addl_args);
                    return;
                }  else  {
                    commands[i].function(NULL);
                }
            }
        }
    }
}


void
unknown_command(void)  {
    transmit("Unknown command\r\n");
}


static int
word_count(const char *str)  {
    register int i;
    int str_len = strlen(str);
    int num_words = 1;
    char last_letter;


    for (i = 0; i < str_len; ++i)  {
        last_letter = str[i-1];
        if (isspace(str[i]) != 0  &&  isspace(last_letter) == 0)  {
            ++num_words;
        }
    }
    return num_words;
}


static int
get_word(const char *str, char *word, const int word_num)  {
    register int i;
    int str_len = strlen(str);
    int cur_word = 1;
    char last_letter;


    for (i = 0; i < str_len; ++i)  {
        if (cur_word == word_num)  {
            strlcpy(word, &str[i], str_len-i+1);
            break;
        }
        last_letter = str[i-1];
        if (isspace(str[i]) != 0  &&  isspace(last_letter) == 0)  {
            ++cur_word;
        }
    }

    for (i = 0; ; ++i)  {
        if (isspace(word[i]) != 0  ||  word[i] == '\0')  {
            word[i] = '\0';
            return 1;
        }
    }

      /* Shouldn't be here... */
    return 0;
}


void
kill_system_wrap(char *addl_args)  {
    kill_system(-1);
    end_transmission();
}


void
restart_system_wrap(char *addl_args)  {
    restart_system(-1);
    end_transmission();
}


void
list_commands(char *addl_args)  {
    register int i;

    for (i = 0; ; ++i)  {
        if (commands[i].function == NULL)  {
            break;
        }  else  {
            transmit("%10s: %s\r\n", commands[i].command, commands[i].description);
        }
    }
    end_transmission();
}


void
terminate_process(char *addl_args)  {
    register int i;
    int addl_args_len = strlen(addl_args) + 1;
    char *proc_c;
    char *sig_c;
    pid_t proc = 0;
    int sig = SIGTERM;
    int num_words = word_count(addl_args);


    if (num_words == 1)  {

        if (isdigit(addl_args[0]) != 0)  {
            kill(atoi(addl_args), SIGTERM);
        }  else  {
            ;  /* failure */
        }

    }  else if (num_words > 1)  {

        sig_c = (char *)malloc(addl_args_len);
        if (sig_c == NULL)  {
            dk_err("Couldn't allocate memory for signal name / number");
        }

        get_word(addl_args, sig_c, 1);
        if (isdigit(sig_c[0]))  {
            sig = atoi(sig_c);
        }  else  {
            for (i = 0; i < NSIG; ++i)  {
                if (!strcasecmp(sys_signame[i], sig_c))  {
                    sig = i;
                    break;
                }
            }
        }

        free(sig_c);

        proc_c = (char *)malloc(addl_args_len);
        if (proc_c == NULL)  {
            dk_err("Couldn't allocate memory for process name / pid");
            return;
        }

        get_word(addl_args, proc_c, 2);
        if (isdigit(proc_c[0]))  {
            proc = (pid_t)atoi(proc_c);
        }  else  {
            ;  /* match process by name */
        }

        free(proc_c);

        kill(proc, sig);


    }

    end_transmission();
}


void
renice_process(char *addl_args)  {
    ;
}
