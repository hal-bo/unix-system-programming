// 61911650 高野遥斗

#include "command.h"

struct command *getcommand()
{
    int c;
    int i = 0;
    struct command *command_head = create_empty_command();
    struct command *cmd = command_head;
    enum TKN_TYPE status = TKN_NORMAL;
    char buf[BUF_SIZE];

    while ((c = getchar()) != EOF && c != '\n')
    {
        if (isspace(c)) {
            continue;
        }
        switch (c)
        {
            case '<':
                status = TKN_REDIR_IN;
                break;
            case '>':
                if ((c = getchar()) == '>') {
                    status = TKN_REDIR_APPEND;
                } else {
                    ungetc(c, stdin);
                    status = TKN_REDIR_OUT;
                }
                break;
            case '|':
                status = TKN_PIPE;
                break;
            case '&':
                command_head->background = 1;
                status = TKN_BG;
                break;
            default:

                i = 0;
                buf[i++] = c;
                while (isalnum(c = getchar()) || c == '-' || c == '.' || c == ',' || c == '/' || c == '_')
                {
                    buf[i++] = c;
                }
                ungetc(c, stdin);
                buf[i++] = '\0';
                if (status == TKN_REDIR_IN) {
                    printf("malloc redir_in\n");
                    cmd->redir_in = (char *)malloc(i * sizeof(char));
                    strcpy(cmd->redir_in, buf);
                } else if (status == TKN_REDIR_OUT) {
                    printf("malloc redir_out\n");
                    cmd->redir_out = (char *)malloc(i * sizeof(char));
                    strcpy(cmd->redir_out, buf);
                } else if (status == TKN_REDIR_APPEND){
                    printf("malloc redir_append\n");
                    cmd->redir_append = (char *)malloc(i * sizeof(char));
                    strcpy(cmd->redir_append, buf);
                } else {
                    if (status == TKN_PIPE) {
                        cmd->next_command = create_empty_command();
                        cmd = cmd->next_command;
                    } else if (status == TKN_NORMAL && cmd->argc > 0) {
                        cmd->argv = (char **)realloc(cmd->argv, (cmd->argc+2) * sizeof(char *));
                    }
                    cmd->argc++;
                    cmd->argv[cmd->argc-1] = (char *)malloc(i * sizeof(char));
                    cmd->argv[cmd->argc] = NULL;
                    strcpy(cmd->argv[cmd->argc-1], buf);
                }
                status = TKN_NORMAL;
        }
    }
    return command_head;
}

void print_command(struct command *cmd)
{
    int i;
    struct command *c;
    for (c = cmd; is_valid_command(c); c = c->next_command) {
        printf("* command *\n");
        for (i=0;i<c->argc;i++){
            printf("%s ", c->argv[i]);
        }
        printf("\n");
        if (c->redir_in) {
            printf("redir_in: %s\n", c->redir_in);
        } else {
            printf("redir_in:\n");
        }
        if (c->redir_out) {
            printf("redir_out: %s\n", c->redir_out);
        } else {
            printf("redir_out:\n");
        }
        if (c->redir_append) {
            printf("redir_append: %s\n", c->redir_append);
        } else {
            printf("redir_append:\n");
        }
        printf("background: %s", c->background ? "true" : "false");
        printf("\n");
    }
    printf("\n");
}


int proc_count(struct command *cmd)
{
    int i = 0;
    for (struct command *c = cmd; is_valid_command(c) && c->argc > 0; c = c->next_command) {
        i++;
    }
    return i;
}

int is_valid_command(struct command *cmd)
{
    return cmd != NULL && cmd->argv != NULL;
}

struct command *create_empty_command()
{
    struct command *cmd = (struct command *)malloc(sizeof(struct command));
    cmd->argc = 0;
    cmd->background = 0;
    cmd->argv = (char **)malloc(2 * sizeof(char *));
    return cmd;
}

void free_command(struct command *cmd)
{
    struct command *next_cmd;
    while (is_valid_command(cmd)) {
        next_cmd = cmd->next_command;
        for (int i=0;i<cmd->argc+1;i++) {
            if (cmd->argv[i] != NULL) {
                free(cmd->argv[i]);
                cmd->argv[i] = NULL;
            }
        }
        if (cmd->redir_in != NULL) {
            free(cmd->redir_in);
            cmd->redir_in = NULL;
        }
        if (cmd->redir_out != NULL) {
            free(cmd->redir_out);
            cmd->redir_out = NULL;
        }
        if (cmd->redir_append != NULL) {
            free(cmd->redir_append);
            cmd->redir_append = NULL;
        }
        free(cmd);
        cmd = next_cmd;
    }
}