#include "header_sh.h"

void    redirect(int oldfd, int newfd)
{
    if (oldfd != newfd)
    {
        if (dup2(oldfd, newfd) != -1)
        {
            if (close(oldfd) == -1)
                perror("close");
        }
        else
            perror("dup2");
    }
}

int     check_redirect_input(char **cmd, char **input_filename)
{
    int i, j;

    for (i = 0; cmd[i] != NULL; i++)
    {
        if (cmd[i][0] == '<')
        {
            free(cmd[i]);

            if (cmd[i + 1] != NULL)
                *input_filename = cmd[i + 1];
            else
                return -1;

            for (j = i; cmd[j - 1] != NULL; j++)
                cmd[j] = cmd[j + 2];

            return TKN_REDIR_IN;
        }
    }

    return 0;
}
int     check_redirect_output(char **cmd, char **output_filename)
{
    int i, j, status;

    for (i = 0; cmd[i] != NULL; i++)
    {
        if (cmd[i][0] == '>')
        {
            if (cmd[i][1] == '>')
                status = TKN_REDIR_APPEND;
            else
                status = TKN_REDIR_OUT;
            free(cmd[i]);

            if (cmd[i + 1] != NULL)
                *output_filename = cmd[i + 1];
            else
                return -1;

            for (j = i; cmd[j - 1] != NULL; j++)
                cmd[j] = cmd[j + 2];

            return status;
        }
    }

    return 0;
}

void    exec_redirect(char **cmd, int *input, int *output, char **input_filename, char **output_filename)
{
    // check refirect input
    if ((*input = check_redirect_input(cmd, input_filename)) == -1)
        fprintf(stderr, "redirect input error\n");
    else if (*input == TKN_REDIR_IN)
        printf("Redirecting input from %s\n", *input_filename);

    // check redirect output or redirect append
    if ((*output = check_redirect_output(cmd, output_filename)) == -1)
        fprintf(stderr, "redirect output error\n");
    else if (*output == TKN_REDIR_APPEND)
        printf("Redirecting output append to %s\n", *output_filename);
    else if (*output == TKN_REDIR_OUT)
        printf("Redirecting output to %s\n", *output_filename);
}
