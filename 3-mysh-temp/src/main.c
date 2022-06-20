// 61911032 Sekiguchi Atsushi
#include "header_sh.h"

int     main()
{
    pid_t   ret;
    int     status = 1;
    int     i, j;

    printf("--- MY SHELL START ---\n\n");

    // sigintSignalIgnore();
    // sigstpSignalIgnore();
    // sigchldSignalIgnore();

    ppid_global = getpid();
	Signal(SIGINT,  sigint_handler);   /* ctrl-c */
	// Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
	Signal(SIGCHLD, sigchld_handler);  

    do
    {

        int     i = 0, j = 0, s;
        int     cmd_index = 0, proc_index = 0;

        int     *tokens;
        mem_alloc(tokens, int, MAX_ARGS, EXIT_FAILURE);

        printf("$ ");

        char    **cmds[MAX_PROCS];
        for (i = 0; i < MAX_PROCS; i++)
            mem_alloc(cmds[i], char *, MAX_ARGS, EXIT_FAILURE);
        

        for (i = 0; i < MAX_ARGS; i++)
            mem_alloc(cmds[proc_index][i], char, TKN_SIZE, EXIT_FAILURE);

        char    *buf;
        mem_alloc(buf, char, TKN_SIZE, EXIT_FAILURE);

        while ((s = gettoken(buf)))
        {
            *tokens = s;

#ifdef DEBUG
            printf("token [%s] \n", tok2opr(*tokens++));
#endif

            if (s == TKN_EOF)
            {
                status = 0;
                break;
            }
            else if (s == TKN_EOL)
            {
                cmds[proc_index][cmd_index] = NULL;
                proc_index++;
                cmd_index = 0;
                cmds[proc_index] = NULL;
                break;
            }
            else if (s == TKN_PIPE)
            {
                cmds[proc_index][cmd_index] = NULL;
                proc_index++;
                cmd_index = 0;

                for (i = 0; i < MAX_ARGS; i++)
                    mem_alloc(cmds[proc_index][i], char, TKN_SIZE, EXIT_FAILURE);
            }
            else 
                strcpy(cmds[proc_index][cmd_index++], buf);
            
            free(buf);
            mem_alloc(buf, char, TKN_SIZE, EXIT_FAILURE);
        }

#ifdef DEBUG
        /* TODO : これいらないから消す, デバッグ用 */
        for (i = 0; i < proc_index; i++)
        {
            printf("command %d   ", i);

            for (j = 0; j < MAX_ARGS; j++)
            {
                printf(" [%s] ", cmds[i][j]);
                if (cmds[i][j] == NULL)
                    break;
                
            }
            printf("\n");
        }
#endif

        if (cmds[0][0] != NULL)
        {
            if (mysh_execute(cmds) == -1)
                fprintf(stderr, "no such command\n");
        }

        for (i = 0; i < MAX_PROCS; i++)
            free(cmds[i]);
        
    } while (status);

    return EXIT_SUCCESS;
}
