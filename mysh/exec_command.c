// 61911650 高野遥斗

#include "command.h"
#include "builtin_cmd.h"

char *merge_path(char *path_a, char *path_b)
{
    int len = strlen(path_a) + strlen(path_b) + 1;
    char *path = (char *)malloc((len+1) * sizeof(char));
    unsigned long i = 0, j = 0;
    while (i < strlen(path_a)) {
        path[i] = path_a[i];
        i++;
    }
    path[i++] = '/';
    while (j < strlen(path_b)) {
        path[i+j] = path_b[j];
        j++;
    }
    path[i+j] = '\0';
    return path;
}

char *get_absolute_path(char *argv)
{
    if (argv[0] == '/' || argv[0] == '.') {
        return (argv);
    }
    char *env_paths = getenv("PATH");
    char *env_path = strtok(env_paths, ":");
    while (env_path) {
        char *path = merge_path(env_path, argv);
        if (access(path, X_OK) == 0) {
            return path;
        }
        env_path = strtok(NULL, ":");
        //free(path);
        path = NULL;
    }
    return NULL;
}

void my_execve(struct command *cmd)
{
    char *abspath = get_absolute_path(cmd->argv[0]);
    if (abspath != NULL) {
        if (execve(abspath, cmd->argv, environ) == -1) {
            perror("execve");
            exit(1);
        }
    } else {
        perror("execve: No such file or directory yead");
        exit(1);
    }
}

void redirect(int oldfd, int newfd)
{
    if (oldfd >= 0 && newfd >= 0 && oldfd != newfd) {
        if (dup2(oldfd, newfd) != -1) {
            safe_close(oldfd);
        } else {
            perror("dup2");
            exit(1);
        }
    }
}

void safe_close(int fd)
{
    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }
}

int open_file(enum TKN_TYPE type, char *filename)
{
    int fd  = -1;

    switch (type) {
        case TKN_REDIR_IN:
            if ((fd = open(filename, O_RDONLY, 0644)) < 0) {
                perror("open");
                exit(1);
            }
            break;
        case TKN_REDIR_OUT:
            if ((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
                perror("open");
                exit(1);
            }
            break;
        case TKN_REDIR_APPEND:
            if ((fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0) {
                perror("open");
                exit(1);
            }
            break;
        default:
            break;
    }
    return fd;
}

void exec_command(struct command *cmd)
{
    int status;//, wpid;
    int pc = proc_count(cmd);
    if (pc == 0) {
        return;
    }
    int pfd[pc-1][2];
    int i = 0;
    if (pc == 1 && exec_builtin_cmd(cmd->argc, cmd->argv) < 0) {
        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            if (cmd->redir_in) {
                int fd = open_file(TKN_REDIR_IN, cmd->redir_in);
                redirect(fd, STDIN_FILENO);
            }
            if (cmd->redir_out) {
                int fd = open_file(TKN_REDIR_OUT, cmd->redir_out);
                redirect(fd, STDOUT_FILENO);
            }
            if (cmd->redir_append) {
                int fd = open_file(TKN_REDIR_APPEND, cmd->redir_append);
                redirect(fd, STDOUT_FILENO);
            }
            my_execve(cmd);
        } else if (cmd->background == 0) {
            wait(&status);
        }
    } else if (pc > 1) {
        struct command *c;
        for (c = cmd; is_valid_command(c);c = c->next_command) {
            if (i != pc - 1) {
                pipe(pfd[i]);
            }
            int pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            } else if (pid == 0) {
                if (i == 0) {
                    safe_close(pfd[i][0]);
                    redirect(pfd[i][1], STDOUT_FILENO);
                } else if (i == pc - 1) {
                    safe_close(pfd[i-1][1]);
                    redirect(pfd[i-1][0], STDIN_FILENO);
                } else {
                    safe_close(pfd[i-1][1]);
                    safe_close(pfd[i][0]);
                    redirect(pfd[i-1][0], STDIN_FILENO);
                    redirect(pfd[i][1], STDOUT_FILENO);
                }
                my_execve(c);
            } else if (i > 0){
                safe_close(pfd[i-1][0]);
                safe_close(pfd[i-1][1]);
            }
            i++;
        }
        if (cmd->background == 0) {
            for (c = cmd; is_valid_command(c);c = c->next_command) {
                wait(&status);
            }
        }
    }
}