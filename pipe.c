/* mshell - a job manager */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "pipe.h"
#include "jobs.h"

void do_pipe(char *cmds[MAXCMDS][MAXARGS], int nbcmd, int bg) {
    int fds[2];
    pid_t pid;

    assert(pipe(fds) != -1);

    switch(pid = fork()){
    	case -1: if (verbose)
    				printf("do_pipe: EXIT_FAILURE, pid = -1\n");
    	case 0: setpgid(0,0);
    			assert(dup2(fds[1], STDOUT_FILENO) != -1);
    			close(fds[0]);
    			close(fds[1]);
    			execvp(cmds[0][0], cmds[0]);

    			if (verbose)
    				printf("do_pipe: EXIT_FAILURE, pid=0\n");

    			exit(EXIT_FAILURE);
  		default: switch(fork()){
  					case -1: if (verbose)
  								printf("do_pipe: EXIT_FAILURE, pid = -1\n");
  					case 0: setpgid(0, pid);
  							assert(dup2(fds[0], STDIN_FILENO) != -1);
  							close(fds[0]);
  							close(fds[1]);
  							execvp(cmds[1][0], cmds[1]);

  							if (verbose)
  								printf("do_pipe: EXIT_FAILURE, pid = 0\n");
  		}
    }

    close(fds[0]);
    close(fds[1]);

    if (verbose)
    	printf("do_pipe: jobs_addjob\n");
    jobs_addjob(pid, bg ? BG : FG, cmds[0][0]);

    if (!bg) {
    	if (verbose)
    		printf("do_pipe: waitfg");
    	waitfg(pid);
    }

    return;
}
