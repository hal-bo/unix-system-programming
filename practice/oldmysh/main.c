// 61911650 高野遥斗

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"
#include "builtin_cmd.h"

#define LINE_LENGTH 256
#define COMMAND_SIZE 256
#define PIPE_SIZE 10

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

typedef struct command {
    int argc;
    char *argv[];
    char *redir_in[];
    char *redir_out[];
    char *redir_out_add[]
    struct command *next_command;
} command;


command *split_command(char *input_line)
{
    char *token = strtok(input_line, TOKEN_PIPE);
    command *command_head = malloc(command);
    command com = command_head;
    while (token != NULL) {
        
        getargs(token, )
        token = strtok(NULL, TOKEN_PIPE);
    }
    return command_head;
}
int main()
{
    int pid, stat;
    int ac;
    char *av[COMMAND_SIZE];

    while (1) {
        commandline(&ac, av);
        int i, pipe_locate[PIPE_SIZE], pipe_count = 0;
        pipe_locate[0] = -1;
        for (i = 0; av[i] != NULL; i++) {
            if (strcmp(av[i], "|") == 0) {
                pipe_count++;
                pipe_locate[pipe_count] = i;
                av[i] = NULL;
            }
        }
        int pfd[PIPE_SIZE-1][2];
        if (pipe_count == 0) {
            if (av[0]) {
                if (exec_builtin_cmd(ac, av) < 0) {
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
                }
                free_argv(ac, av);
            }
        }
        for (i = 0; i < pipe_count + 1 && pipe_count != 0; i++) {
            if (i != pipe_count) {
                pipe(pfd[i]);  //最後のコマンドでなければパイプを作成
            }
            if (fork() == 0) {
                //子プロセス
                if (i == 0) {
                    //最初のコマンドなので、標準出力をパイプの入り口へつなげる
                    dup2(pfd[i][1], 1);  
                    close(pfd[i][0]); close(pfd[i][1]);
                } else if (i == pipe_count) {
                    //最後のコマンドなので、標準入力をパイプの出口へつなげる
                    dup2(pfd[i - 1][0], 0);
                    close(pfd[i - 1][0]); close(pfd[i - 1][1]);
                } else {
                    //途中のコマンドなので、上記の処理を両方やる
                    dup2(pfd[i - 1][0], 0);
                    dup2(pfd[i][1], 1);
                    close(pfd[i - 1][0]); close(pfd[i - 1][1]);
                    close(pfd[i][0]); close(pfd[i][1]);
                }
                execvp(av[pipe_locate[i] + 1], av + pipe_locate[i] + 1);
                exit(0);
            }
            else if (i > 0) {
                //親プロセス
                //つなげ終わったパイプは閉じる
                close(pfd[i - 1][0]); close(pfd[i - 1][1]);
            }
        }
        for (i = 0; i < pipe_count + 1; i++) {
            wait(&stat);
        }
    }
    return 0;
}