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
 * $Id: proc.c,v 1.1.1.1 2004/10/26 14:38:03 sjg Exp $
 */


#include "proc.h"
#include "error.h"



/*
 * kill all processes for the prison.
 */
void
kill_procs(void)  {
    int procs_left = 0;       /* Number of processes that didn't TERMinate */
    int num_procs;
    struct process_info *sp = NULL;
    pid_t my_pid = getpid();


      /* We play nice */
    kill(-1, SIGTERM);
    sleep(1);

    num_procs = get_proc_info(&sp);

    if (num_procs != 0)  {
        while (sp->pid != -1)  {
            if (my_pid == sp->pid)  {
                break;
            }  else  {
                printf("%s (%d) didn't die, will send SIGKILL in 5 seconds.\n",
                        sp->name, sp->pid);
                ++procs_left;
            }
            ++sp;
        }

        if (procs_left > 0)  {
              /* They might possibly exit gracefully if we give
                 them a little time ... */
            sleep(5);

              /* Get tough */
            kill(-1, SIGKILL);
        }
        free_get_proc_info(sp);
    }
}


/*
* Calling this function will generate a list of all
* processes on the system and return a malloc'd pointer
* to the process structure, the end of the list being
* marked by a process with a pid of -1.
*/
int
get_proc_info(struct process_info **ret_sp)  {
    struct kinfo_proc *kp = NULL, *newkp;
    struct process_info *sp;
    register int i;
    int num_proc, st;
    int mib[4];
    size_t miblen, size;


      /* Tell sysctl what we want */
    size = 0;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;
    miblen = 3;

      /* Populate kp with process info */
    st = sysctl(mib, miblen, NULL, &size, NULL, 0);
    if (st == -1)  {
        dk_err("sysctl(3) for kinfo_proc structure size failed");
        return 0;
    }

    size += size / 10;
    newkp = realloc(kp, size);

    if (newkp == 0)  {
        if(kp)  {
            free(kp);
        }
        dk_errx("Memory re-allocation for kinfo_proc failed");
    }

    kp = newkp;
    st = sysctl(mib, miblen, kp, &size, NULL, 0);
    if (st == -1)  {
        dk_err("Could not sysctl(KERN_PROC)");
        return 0;
    }

      /* Get number of processes and make room to store info about them */
    num_proc = size / sizeof(struct kinfo_proc);
    sp = (struct process_info *)malloc(sizeof(struct process_info)*(num_proc+1));
    if (sp == NULL)  {
        dk_errx("Memory allocation for process_info structure failed");
    }

      /* Put info we want into structure we got pointer to */
    for (i = 0; i < num_proc; ++i)  {
#if __FreeBSD_version > 500016
        strncpy(sp[i].name, kp[i].ki_ocomm, MAXCOMLEN);
        sp[i].pid = kp[i].ki_pid;
        sp[i].cpu_time = kp[i].ki_estcpu;
#else
        strncpy(sp[i].name, kp[i].kp_proc.p_comm, MAXCOMLEN);
        sp[i].pid = kp[i].kp_proc.p_pid;
        sp[i].cpu_time = kp[i].kp_proc.p_cpticks;
#endif
    }

    free(kp);

      /* Set the pid field at the end to -1, so that
         a loop using this info knows when to stop */
    sp[i].pid = -1;
    *ret_sp = sp;
    return num_proc;
}

void
free_get_proc_info(struct process_info *sp)  {
    sp = 0;
    free(sp);
}
