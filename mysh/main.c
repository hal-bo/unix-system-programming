// 61911650 高野遥斗

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

#define LINE_LENGTH 256
#define COMMAND_SIZE 256
void exit_proc(int, char *[]), cd_proc(int, char *[]);

struct built_in_command_table {
    char *cmd;
    void (*func)(int, char *[]);
}
cmd_tbl[] = {
    {"exit", exit_proc},
    {"cd", cd_proc},
    {NULL, NULL}
};

void exit_proc(int argc, char *argv[])
{
    argc++;
    argv++;
    exit(0);
}

void cd_proc(int argc, char *argv[])
{
    if (argc == 2) {
        printf("%s\n", argv[1]);
        if (chdir(argv[1]) < 0) {
            perror(argv[0]);
        }
    } else if (argc == 1) {
        printf("%s\n", getenv("HOME"));
        if (chdir(getenv("HOME")) < 0) {
            perror(argv[0]);
        } 
    }
}
void free_argv(int argc, char *argv[])
{
    int i;
    
    for (i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
}

void commandline(int *argc, char *argv[])
{
    char lbuf[LINE_LENGTH];

    printf("mysh$ ");
    if (fgets(lbuf, LINE_LENGTH, stdin) == NULL) {
        return;
    }
    getargs(lbuf, argc, argv);
}

int main()
{
    struct built_in_command_table *p;
    int pid, stat;
    int ac;
    char *av[COMMAND_SIZE];

    while (1) {
        commandline(&ac, av);
        if (av[0]) {
            for (p = cmd_tbl; p->cmd; p++) {
                if (strcmp(av[0], p->cmd) == 0) {
                    (*p->func)(ac, av);
                    break;
                }
            }
            if ((pid = fork()) < 0) {
                perror("fork");
            } else if (pid == 0) {
                if (execvp(av[0], av) < 0) {
                    perror("execvp");
                    exit(1);
                }
            } else {
                wait(&stat);
            }
            free_argv(ac, av);
        }
    }
    return 0;
}