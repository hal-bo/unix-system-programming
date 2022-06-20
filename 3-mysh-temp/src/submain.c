#include "header_sh.h"

char    *tok2opr(int token)
{
    switch (token)
    {
    case TKN_BG:
        /* code */
        return "TKN_BG";
    case TKN_EOF:
        return "TKN_EOF";
    case TKN_EOL:
        return "TKN_EOL";
    case TKN_REDIR_APPEND:
        return "TKN_REDIR_APPEND";
    case TKN_REDIR_IN:
        return "TKN_REDIR_IN";
    case TKN_REDIR_OUT:
        return "TKN_REDIR_OUT";
    case TKN_PIPE:
        return "TKN_PIPE";
    case TKN_ERROR:
        return "TKN_ERROR";
    case TKN_STR:
        return "TKN_STR";
    default:
        return NULL;
    }
}

int     mysh_execute(char ***cmds)
{
    int     i;
    struct command_table    *p;

    // build-in command
    for (p = cmd_tbl; p->cmd; p++)
    {
        if (strcmp(cmds[0][0], p->cmd) == 0)
        {
            int argc = 0;
            for (i = 0; i < MAX_ARGS; i++)
            {
                if (cmds[0][i] == NULL)
                    break;
                argc++;
            }
            (*p->func)(argc, cmds[0]);
            return 1;
        }
    }

    // command which use execve
    if (p->cmd == NULL)
    {
        mysh_launch(cmds);
        return 1;
    }

    // otherwise
    return -1;
}

int     mysh_launch(char ***cmds)
{
    pid_t   pid, wpid;
    int     status;
    int     block;

    // check ampersand
    block = (check_ampersand(cmds) == 0);

#ifdef DEBUG
    printf("block is %d\n", block);
#endif

    pid = fork();
    if (pid == 0)
        exec_pipeline(cmds, 0, STDIN_FILENO);
    else if (pid < 0)
        perror("mysh");
    else
    {
        if (block)
        {
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }

    return 1;
}

int     gettoken(char *buf)
{
    int c, at = 0, status;
    while ((c = getchar()) != EOF)
    {
        switch (c)
        {
        case ' ':
            break;
        case '\n':
            buf[at++] = '\0';
            status = TKN_EOL;
            return status;
        case '|':
            buf[at++] = '\0';
            status = TKN_PIPE;
            return status;
        case '&':
            status = TKN_BG;
            buf[at++] = c;
            buf[at++] = '\0';
            return status;
        case '<':
            status = TKN_REDIR_IN;
            buf[at++] = c;
            buf[at++] = '\0';
            return status;
        case '>':
            buf[at++] = c;
            if ((c = getchar()) == '>')
            {
                buf[at++] = c;
                buf[at++] = '\0';
                status = TKN_REDIR_APPEND;
            }
            else
            {
                ungetc(c, stdin);
                buf[at++] = '\0';
                status = TKN_REDIR_OUT;
            }
            return status;
        default:
            if (isalnum(c) || c == '-' || c == '.' || c == '~')
            {
                buf[at++] = c;
                while (isalnum(c = getchar()) || c == '-' || c == '.' || c == ',' || c == '/' || c == '_')
                {
                    buf[at++] = c;
                }
                buf[at++] = '\0';
                ungetc(c, stdin);
                status = TKN_STR;
            }
            else
            {
                /* それ以外 */
            }
            return status;
        }
    }

    /* Ctrl + D が入力されたときにこっちにくる*/
    status = TKN_EOF;
    return status;
}
