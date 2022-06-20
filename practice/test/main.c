// 61911650 高野遥斗

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split_line.h"

#define TOKEN_DELIM " \t\r\n\a"

typedef struct command {
    int argc;
    char *argv[];
    char *redir_in[];
    char *redir_out[];
    char *redir_out_add[]
    struct command *next_command;
} command;


void split_proc(int in_argc, char *in_argv[], int proc_index, 
int *proc_argc, char *proc_argv[], char redir_in[], char redir_out, char redir_out_add[])
{

}
int main()
{
    char input_line[] = "ls > wc";
    //input_line = NULL;
    // char *command_token = strtok(input_line, TOKEN_DELIM);
    // //command_token = NULL;
    // printf("command_token: %s\n", command_token);
    // command_token = strtok(NULL, TOKEN_DELIM);
    // printf("command_token: %s\n", command_token);
    // command_token = strtok(NULL, TOKEN_DELIM);
    // printf("command_token: %s\n", command_token);
    // command_token = strtok(NULL, TOKEN_DELIM);
    // printf("command_token: %s\n", command_token);
    line *command_line = split_line(input_line);
    int i = 1;
    for (line *l = command_line; l != NULL; l = l->next_line) {
        printf("line %d\n", i++);
        int j = 1;
        for (block *b = &(l->blk); b != NULL; b = b->next_block) {
            printf("block %d\n", j++);
            printf("token %s\n", b->tok);
        }
    }
    return 0;
}