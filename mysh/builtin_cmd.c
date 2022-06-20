// 61911650 高野遥斗

# include "builtin_cmd.h"

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

int exec_builtin_cmd(int argc, char *argv[])
{
    struct built_in_command_table *p;

    for (p = cmd_tbl; p->cmd; p++) {
        if (strcmp(argv[0], p->cmd) == 0) {
            (*p->func)(argc, argv);
            return 0;
        }
    }
    return -1;
}