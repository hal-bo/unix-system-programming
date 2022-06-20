#define MYSH_H
#ifndef MYSH_H

typedef struct command {
    int argc = 1;
    char *argv[];
    char *redir_in;
    char *redir_out;
    char *redir_append;
    int background = false;
    struct command *next_command;
} command;

#endif