// 61911650 高野遥斗

#include <stdio.h>
#include <string.h>
#include "string_utils.h"
#include "buffer.h"
#define LINE_LENGTH 256
#define COMMAND_SIZE 256

void help_proc(int, char *[]), init_proc(int, char *[]), buf_proc(int, char *[]);
void hash_proc(int, char *[]), free_proc(int, char *[]), getblk_proc(int, char *[]);
void brelse_proc(int, char *[]), set_proc(int, char *[]), reset_proc(int, char *[]);
void quit_proc(int, char *[]);

struct command_table {
    char *cmd;
    void (*func)(int, char *[]);
}
cmd_tbl[] = {
    {"help", help_proc},
    {"init", init_proc},
    {"buf", buf_proc},
    {"hash", hash_proc},
    {"free", free_proc},
    {"getblk", getblk_proc},
    {"brelse", brelse_proc},
    {"set", set_proc},
    {"reset", reset_proc},
    {"quit", quit_proc},
    {NULL, NULL}
};

/**
 * @fn
 * malloc した argv のメモリを解放する関数。
 * コマンド実行後に必ず呼び出すようにする。
 */
void free_argv(int argc, char *argv[])
{
    int i;
    
    for (i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
}

void help_proc(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else {
        printf(
            "\n"
            "init\n"
            "initialize hashlists and free lists.\n"
            "\n"
            "buf [n ...]\n"
            "print the status of all buffers. If an argument is specified, "
            "print the status of the buffer whose number associated with the number "
            "(not the logical block number associated with the buffer) is n.\n"
            "\n"
            "hash [n ...]\n"
            "print all hashlists. If an argument is specified, print hashlists with hash value n.\n"
            "\n"
            "free\n"
            "print freelists.\n"
            "\n"
            "getblk n\n"
            "execute getblk(n) with logical block number n as an argument.\n"
            "\n"
            "brelse n\n"
            "execute brelse(bp) with the argument bp, "
            "a pointer to the buffer header corresponding to the logical block number n\n"
            "\n"
            "set n stat [stat ...]\n"
            "set state stat for buffer with logical block number n.\n"
            "\n"
            "reset n stat [stat ...]\n"
            "reset state stat for buffer with logical block number n.\n"
            "\n"
            "quit\n"
            "exit this software.\n"
            "\n"
        );
    }
}

void init_proc(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else {
        init_cmd();
    }
}

void buf_proc(int argc, char *argv[])
{
    int i;

    if (argc >= 2) {
        for (i=1;i<argc;i++){
            if (is_str_numeric(argv[i])) {
                buf1_cmd(atoi(argv[i]));
            } else {
                fprintf(stderr, "%s is invalid\n", argv[i]);
            }
        }
    } else {
        buf_cmd();
    }
}

void hash_proc(int argc, char *argv[])
{
    int i;

    if (argc >= 2) {
        for (i=1;i<argc;i++){
            if (strlen(argv[i]) == 1 && '0' <= argv[i][0] && argv[i][0] < '0' + NHASH) {
                hash1_cmd(atoi(argv[i]));
            } else {
                fprintf(stderr, "%s is invalid\n", argv[i]);
            }
        }
    } else {
        hash_cmd();
    }
}

void free_proc(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else {
        free_cmd();
    }
}

void getblk_proc(int argc, char *argv[])
{
    if (argc > 3) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else if (argc == 2) {
        if (is_str_numeric(argv[1])) {
            getblk_cmd(atoi(argv[1]));
        } else {
            fprintf(stderr, "%s is invalid\n", argv[1]);
        }
    } else {
        fprintf(stderr, "%s: few arguments\n", argv[0]);
    }
}

void brelse_proc(int argc, char *argv[])
{
    if (argc > 3) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else if (argc == 2) {
        if (is_str_numeric(argv[1])) {
            brelse_cmd(atoi(argv[1]));
        } else {
            fprintf(stderr, "%s is invalid\n", argv[1]);
        }
    } else {
        fprintf(stderr, "%s: few arguments\n", argv[0]);
    }
}

void set_proc(int argc, char *argv[])
{
    int i;

    if (argc < 3) {
        fprintf(stderr, "%s: few arguments\n", argv[0]);
    } else {
        if (is_str_numeric(argv[1])) {
            for (i = 2; i < argc; i++) {
                if (strlen(argv[i]) == 1) {
                    set_cmd(atoi(argv[1]), argv[i][0]);
                } else {
                    fprintf(stderr, "%s is invalid\n", argv[i]);
                }
            }
        } else {
            fprintf(stderr, "%s is invalid\n", argv[1]);
        }
    }
}

void reset_proc(int argc, char *argv[])
{
    int i;

    if (argc < 3) {
        fprintf(stderr, "%s: few arguments\n", argv[0]);
    } else {
        if (is_str_numeric(argv[1])) {
            for (i = 2; i < argc; i++) {
                if (strlen(argv[i]) == 1) {
                    reset_cmd(atoi(argv[1]), argv[i][0]);
                } else {
                    fprintf(stderr, "%s is invalid\n", argv[i]);
                }
            }
        } else {
            fprintf(stderr, "%s is invalid\n", argv[1]);
        }
    }
}

void quit_proc(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
    } else {
        quit_cmd();
    }
}

void commandline(int *argc, char *argv[])
{
    char lbuf[LINE_LENGTH];

    printf("$ ");
    if (fgets(lbuf, LINE_LENGTH, stdin) == NULL) {
        return;
    }
    getargs(lbuf, argc, argv);
}

int main()
{
    struct command_table *p;
    int ac;
    char *av[COMMAND_SIZE];

    auto_init();
    while (1) {
        commandline(&ac, av);
        if (av[0]) {
            for (p = cmd_tbl; p->cmd; p++) {
                if (strcmp(av[0], p->cmd) == 0) {
                    (*p->func)(ac, av);
                    break;
                }
            }
            if (p->cmd == NULL) {
                fprintf(stderr, "unknown command: %s\n", av[0]);
            }
            free_argv(ac, av);
        }
    }
    return 0;
}