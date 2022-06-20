#include "header_sh.h"


handler_t   *Signal(int signum, handler_t *handler) 
{
	struct sigaction action, old_action;

    memset(&action, 0, sizeof(action));
	action.sa_handler = handler;  
	sigemptyset(&action.sa_mask); 

    if (signum == SIGCHLD) 
        action.sa_flags = SA_NOCLDSTOP | SA_RESTART; 
    if (signum == SIGTSTP) {
        action.sa_flags = 0; 
    }
    if (signum == SIGINT)
        action.sa_flags = SA_RESTART; 

	if (sigaction(signum, &action, NULL) < 0) {
		perror("Signal error");
        exit(EXIT_FAILURE);
    }

	return (old_action.sa_handler);
}

void    sigchld_handler(int sig)
{
    int status = 0;
    pid_t wpid = 0;

    do {
        wpid = waitpid(-1, &status, WNOHANG);
    } while (wpid > 0);

    return;
}


void    sigint_handler(int sig)
{
    write(STDERR_FILENO, " caught SIGINT\n", 14);
}


void    sigtstp_handler(int sig)
{
    write(STDERR_FILENO, " caught SIGTSTP\n", 15);
    pid_t pid;
    pid = getpid();
    if (pid != 0)
    {
        if (kill(-pid, SIGTSTP) < 0) 
            perror("kill error");
    }
}
