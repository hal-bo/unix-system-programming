// 61911650 高野遥斗

#include "command.h"
#include "builtin_cmd.h"
#include "signal.h"

void    sigint_handler()
{
}

int main()
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
	action.sa_handler = sigint_handler;  
	sigemptyset(&action.sa_mask); 
    action.sa_flags = SA_RESTART; 

	if (sigaction(SIGINT, &action, NULL) < 0) {
		perror("sigaction");
        exit(1);
    }

    while (1) {
        printf("$ ");
        struct command *cmd = getcommand();
        print_command(cmd);
        // printf("--------------------\n");
        exec_command(cmd);
        //printf("--------------------\n");
        free_command(cmd);
    }
}