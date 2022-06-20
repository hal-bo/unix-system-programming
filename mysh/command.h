// 61911650 高野遥斗

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/wait.h> 
#define BUF_SIZE 256

extern char **environ;

enum TKN_TYPE {
    TKN_NORMAL,
    TKN_REDIR_IN,
    TKN_REDIR_OUT,
    TKN_REDIR_APPEND,
    TKN_PIPE,
    TKN_BG,
    TKN_EOL,
    TKN_EOF
};
typedef struct command {
    int argc;
    char **argv;
    char *redir_in;
    char *redir_out;
    char *redir_append;
    int background;
    struct command *next_command;
} command;

struct command *getcommand();
void print_command(struct command *cmd);
int proc_count(struct command *cmd);
int is_valid_command(struct command *cmd);
struct command *create_empty_command();
void free_command(struct command *cmd);

void redirect(int oldfd, int newfd);
void safe_close(int fd);
void exec_command(struct command *cmd);