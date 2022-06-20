#include "header_sh.h"

void    exec_pipeline(char ***cmds, int pos, int in_fd)
{
    int     output, input, fdin, fdout;
    char    *output_filename;
    char    *input_filename;

    exec_redirect(cmds[pos], &input, &output, &input_filename, &output_filename);

    if (cmds[pos + 1] == NULL)
    {
        if (input)
        {
            if ((in_fd = open(input_filename, O_RDONLY, 0644)) < 0)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
        }
        redirect(in_fd, STDIN_FILENO);

        if (output == TKN_REDIR_OUT)
        {
            if ((fdout = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
            {
                if (errno != EEXIST)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
            }
            redirect(fdout, STDOUT_FILENO);
        }

        if (output == TKN_REDIR_APPEND)
        {
            if ((fdout = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            redirect(fdout, STDOUT_FILENO);
        }

        char *abspath;
        abspath = get_absolute_path(cmds[pos][0]);

        if (execve(abspath, cmds[pos], environ) == -1)
            perror("exec_pipeline execvp last");
        exit(EXIT_FAILURE);
    }
    else
    {
        pid_t ret;
        int fd[2];                                                          /* output pipe */

        if (pipe(fd) == -1)
            perror("pipe");

        ret = fork();
        if (ret == -1)
            perror("fork");
        else if (ret == 0)
        {                                                                   /* child process */
            if (close(fd[0]) == -1)
                perror("close");

            if (input)
            {
                if ((in_fd = open(input_filename, O_RDONLY, 0644)) < 0)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
            }
            redirect(in_fd, STDIN_FILENO);

            if (output == TKN_REDIR_OUT)
            {
                if ((fd[1] = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
                {
                    if (errno != EEXIST)
                    {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            if (output == TKN_REDIR_APPEND)
            {
                if ((fd[1] = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
            }
            redirect(fd[1], STDOUT_FILENO);

            char *abspath;
            abspath = get_absolute_path(cmds[pos][0]);

            if (execve(abspath, cmds[pos], environ) == -1)
                perror("exec_pipeline execvp last");
            exit(EXIT_FAILURE);
        }
        else
        {
            if (close(fd[1]) == -1)
                perror("close");
            if (close(in_fd) == -1)
                perror("close");

            exec_pipeline(cmds, pos + 1, fd[0]);
        }
    }
}