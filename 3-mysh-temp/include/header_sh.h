#ifndef INCLUDED_HEAD
#define INCLUDED_HEAD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define     BUF_SIZE 1024   
#define     TKN_SIZE 16     
#define     MAX_ARGS 16     
#define     MAX_PROCS 8     

#define     TKN_STR 30
#define     TKN_REDIR_IN 31
#define     TKN_REDIR_OUT 32
#define     TKN_REDIR_APPEND 33
#define     TKN_PIPE 34
#define     TKN_BG 35
#define     TKN_EOL 36
#define     TKN_EOF 37
#define     TKN_ERROR -1
#define     TKN_OPTION 38

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

/* memory allocation */
#define mem_alloc(ptr, type, size, errno)                                                \
    do                                                                                   \
    {                                                                                    \
        if ((ptr = (type *)malloc(sizeof(type) * (size))) == NULL)                       \
        {                                                                                \
            fprintf(stderr, "mysh: cannot allocate %ldbyte. \n", sizeof(type) * (size)); \
            exit(errno);                                                                 \
        }                                                                                \
    } while (0)

#define mem_realloc(ptr, type, size, errno)                                              \
    do                                                                                   \
    {                                                                                    \
        if ((ptr = (type *)realloc(ptr, sizeof(type) * (size))) == NULL)                 \
        {                                                                                \
            fprintf(stderr, "mysh: cannot allocate %ldbyte. \n", sizeof(type) * (size)); \
            exit(errno);                                                                 \
        }                                                                                \
    } while (0)

/* 環境変数取り込み */
extern char **environ;

/* signal_handler */
static int      waiting = 0;
static pid_t    ppid_global;
// extern void     handle_SIGTSTP(int sig_no);
typedef void    handler_t(int);
handler_t       *Signal(int signum, handler_t *handler);
// extern void     sigintSignalIgnore();
// extern void     sigstpSignalIgnore();
// extern void     sigchldSignalIgnore();
extern void     sigchld_handler(int sig);
extern void     sigint_handler(int sig);
extern void     sigtstp_handler(int sig);

/* submain */
extern char     *tok2opr(int token);
extern char     *mysh_read_line(void);
extern char     **mysh_split_line(int *argc, char *line);
extern int      mysh_launch(char ***cmds);
// extern int mysh_execute(int *argc, char **args);
extern int      mysh_execute(char ***cmds);
extern int      gettoken(char *buf);

/* pipe */
extern void     exec_pipeline(char ***cmds, int pos, int in_fd);

/* redirect */
extern void     redirect(int oldfd, int newfd);
extern void     exec_redirect(char **cmd, int *input, int *output, char **input_filename, char **output_filename);
extern int      check_redirect_input(char **cmd, char **input_filename);
extern int      check_redirect_output(char **cmd, char **output_filename);

/* my_execve */
extern char     *help_slash_merge(char const *origin, char const *other);
extern char     *substr(char const *s, unsigned int start, size_t len);
extern char     *get_absolute_path(char *cmd);

/* my_ampersand */
int             check_ampersand(char ***cmds);

/* command */
extern void help_proc(int, char *[]),
    cd_proc(int, char *[]),
    exit_proc(int, char *[]),
    pwd_proc(int, char *[]),
    jobs_proc(int, char *[]),
    fg_proc(int, char *[]),
    bg_proc(int, char *[]);

static struct command_table
{
    char *cmd;
    void (*func)(int, char *[]);
} cmd_tbl[] = {
    {"help", help_proc},
    {"cd", cd_proc},
    {"exit", exit_proc},
    {"pwd", pwd_proc},
    {"jobs", jobs_proc},
    {"fg", fg_proc},
    {"bg", bg_proc},
    {NULL, NULL},
};

#endif
