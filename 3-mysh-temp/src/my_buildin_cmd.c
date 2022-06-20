#include "header_sh.h"

void    help_proc(int ac, char *av[])
{
}

void    exit_proc(int ac, char *av[])
{
    exit(EXIT_SUCCESS);
}

void    cd_proc(int ac, char *av[])
{

    if (av[1] == NULL)
    {
        if (chdir(getenv("HOME")) != 0)
            perror("chdir");
        printf("changed directory\n");
    }
    else
    {
        if (strcmp(av[1], "~") == 0)
        {
            if (chdir(getenv("HOME")) != 0)
                perror("chdir");
        }
        else if (av[1][0] == '~')
        {
            char *dir;
            dir = &av[1][2];
            char *file;
            file = help_slash_merge(getenv("HOME"), dir);

#ifdef DEBUG
            printf("strlen : %d\n", strlen(av[1]) - 3);
            printf("dir : %s\n", dir);
            printf("file : %s\n", file);
#endif
            if (chdir(file) != 0)
                perror("chdir");
            free(file);
        }
        else
        {
            if (chdir(av[1]) != 0)
                perror("chdir");
        }
        printf("changed directory\n");
    }
}

void    pwd_proc(int ac, char *av[])
{
    char pathname[BUF_SIZE];
    memset(pathname, '\0', BUF_SIZE);
    if (ac > 1)
        fprintf(stderr, "this command cannot accept argument\n");
    else
    {
        if (getcwd(pathname, BUF_SIZE) != NULL)
            printf("current directory : %s\n", pathname);
    }
}

void    jobs_proc(int ac, char *av[])
{
}

void    fg_proc(int ac, char *av[])
{
    pid_t shellpid = getpid();
    pid_t pidnumber;
    pid_t parentpid;
    pidnumber = atoi(av[1]);

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    if ((parentpid = getpgid(pidnumber)) == -1)
    {
        perror("getpdid");
        exit(EXIT_FAILURE);
    }

    printf("parent %d\n", parentpid);
    tcsetpgrp(0, parentpid);

    kill(pidnumber, SIGCONT);

    waitpid(pidnumber, NULL, WUNTRACED);

    printf("parent %d \n", shellpid);
    tcsetpgrp(0, shellpid);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
}

void    bg_proc(int ac, char *av[])
{
    pid_t pidnumber;
    pidnumber = atoi(av[1]);
    printf("PID: %d \n", pidnumber);
    kill(pidnumber, SIGCONT);
}