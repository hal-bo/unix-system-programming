#include "myftp.h"

void quit_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    struct myftph_data rmsg;
    argc++;
    argv++;
    struct myftph_data *message = create_message(MYFTPH_TYPE_QUIT, MYFTPH_CODE_IGNORE, 0, NULL);
    send_packet(sock_fd, message, sizeof(struct myftph));
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type != MYFTPH_TYPE_OK || rmsg.code != MYFTPH_CODE_OK_NORMAL) {
        fprintf(stderr, "unexpected packet\n");
    }
    free(message);
    close(sock_fd);
    exit(0);
}

void pwd_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    struct myftph_data rmsg;
    int i;
    if (argc != 1) {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    struct myftph_data *message = create_message(MYFTPH_TYPE_PWD, MYFTPH_CODE_IGNORE, 0, NULL);
    send_packet(sock_fd, message, sizeof(struct myftph));
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type != MYFTPH_TYPE_OK || rmsg.code != MYFTPH_CODE_OK_NORMAL) {
        fprintf(stderr, "unexpected packet\n");
    } else {
        for (i=0;i<rmsg.length;i++){
            printf("%c", rmsg.data[i]);
        }
        printf("\n");
    }
    free(message);
}

void cd_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    struct myftph_data rmsg;
    char *path;
    if (argc < 2) {
        fprintf(stderr, "few arguments\n");
        return;
    } else if (argc == 2) {
        path = argv[1];
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    int length = strlen(path);
    struct myftph_data *message = create_message(MYFTPH_TYPE_CWD, MYFTPH_CODE_IGNORE, length, path);
    send_packet(sock_fd, message, sizeof(struct myftph)+length);
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type == MYFTPH_TYPE_OK) {
        if (rmsg.code != MYFTPH_CODE_OK_NORMAL) {
            fprintf(stderr, "unexpected code\n");
        }
    } else {
        print_error(rmsg);
    }
    free(message);
}

void dir_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    struct myftph_data rmsg;
    struct myftph_data *message;
    char *path;
    int length;
    char buf[BUFSIZE];
    int buf_length = 0;
    int i;
    if (argc < 2) {
        length = 0;
        message = create_message(MYFTPH_TYPE_LIST, MYFTPH_CODE_IGNORE, 0, NULL);
    } else if (argc == 2) {
        path = argv[1];
        length = strlen(path);
        message = create_message(MYFTPH_TYPE_LIST, MYFTPH_CODE_IGNORE, length, path);
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    memcpy(buf, "", BUFSIZE);
    send_packet(sock_fd, message, sizeof(struct myftph)+length);
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type == MYFTPH_TYPE_OK) {
        receive_packet(sock_fd, &rmsg);
        while (rmsg.code == MYFTPH_CODE_DATA_CONTINUE) {
            memcpy(buf+buf_length, rmsg.data, rmsg.length);
            buf_length += rmsg.length;
            receive_packet(sock_fd, &rmsg);
        }
        if (rmsg.code == MYFTPH_CODE_DATA_FINISH) {
            memcpy(buf+buf_length, rmsg.data, rmsg.length);
            buf[buf_length+rmsg.length] = '\0';
            printf("%s", buf);
        } else {
            fprintf(stderr, "unexpected code\n");
        }
    } else {
        print_error(rmsg);
    }
    free(message);
}

void lpwd_cmd(int argc, char *argv[])
{
    char wd[DATASIZE];
    getcwd(wd, DATASIZE);
    printf("%s\n", wd);
}

void lcd_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "input pathname\n");
        return;
    } else if (argc == 2) {
        if (chdir(argv[1]) < 0) {
            perror("lcd");
        }
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
}

void ldir_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    char *path;
    char buf[BUFSIZE];
    memcpy(buf, "", BUFSIZE);
    if (argc < 2) {
        myls(".", buf);
    } else if (argc == 2) {
        myls(argv[1], buf);
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    printf("%s", buf);
}

void get_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    int buf_length = 0;
    int file_fd;
    struct myftph_data rmsg;
    struct myftph_data *message;
    char buf[BUFSIZE];
    char *cpath;
    char *spath;
    if (argc < 3) {
        fprintf(stderr, "few arguments\n");
        return;
    } else if (argc == 2) {
        spath = argv[1];
        cpath = argv[1];
    } else if (argc == 3) {
        spath = argv[1];
        cpath = argv[2];
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    if ((file_fd = open(cpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("open");
        return;
    }
    message = create_message(MYFTPH_TYPE_RETR, MYFTPH_CODE_IGNORE, strlen(spath), spath);
    send_packet(sock_fd, message, sizeof(struct myftph)+strlen(spath));
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type == MYFTPH_TYPE_OK) {
        if (rmsg.code == MYFTPH_CODE_OK_SC) {
            receive_packet(sock_fd, &rmsg);
            while (rmsg.code == MYFTPH_CODE_DATA_CONTINUE) {
                memcpy(buf+buf_length, rmsg.data, rmsg.length);
                buf_length += rmsg.length;
                receive_packet(sock_fd, &rmsg);
            }
            if (rmsg.code == MYFTPH_CODE_DATA_FINISH) {
                memcpy(buf+buf_length, rmsg.data, rmsg.length);
                if (write(file_fd, buf, buf_length+rmsg.length) < 0) {
                    perror("write");
                }
            } else {
                fprintf(stderr, "unexpected code\n");
            }
        } else {
            fprintf(stderr, "unexpected code\n");
        }
    } else {
        print_error(rmsg);
    }
    close(file_fd);
    free(message);
}

void put_cmd(int argc, char *argv[])
{
    extern int sock_fd;
    int n, send_length;
    int file_fd;
    struct myftph_data rmsg;
    struct myftph_data *message;
    struct stat st;
    char buf[BUFSIZE];
    char *cpath;
    char *spath;
    if (argc < 2) {
        fprintf(stderr, "few arguments\n");
        return;
    } else if (argc == 2) {
        cpath = argv[1];
        spath = argv[1];
    } else if (argc == 3) {
        cpath = argv[1];
        spath = argv[2];
    } else {
        fprintf(stderr, "too many arguments\n");
        return;
    }
    if (stat(cpath, &st) < 0) {
        perror("stat");
        return;
    }
    if ((file_fd = open(cpath, O_RDONLY)) < 0) {
        perror("open");
        return;
    }
    message = create_message(MYFTPH_TYPE_STOR, MYFTPH_CODE_IGNORE, strlen(spath), spath);
    send_packet(sock_fd, message, sizeof(struct myftph)+strlen(spath));
    receive_packet(sock_fd, &rmsg);
    if (rmsg.type == MYFTPH_TYPE_OK) {
        if (rmsg.code == MYFTPH_CODE_OK_CS) {
            if (st.st_size > 0) {
                while (st.st_size > send_length) {
                    if (st.st_size - send_length > DATASIZE) {
                        if ((n = read(file_fd, buf, DATASIZE)) < 0) {
                            perror(cpath);
                            close(file_fd);
                            return;
                        }
                        message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_CONTINUE, DATASIZE, buf);
                        send_packet(sock_fd, message, sizeof(struct myftph)+DATASIZE);
                        send_length += DATASIZE;
                    } else {
                        if ((n = read(file_fd, buf, st.st_size - send_length)) < 0) {
                            perror(cpath);
                            close(file_fd);
                            return;
                        }
                        message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, st.st_size-send_length, buf);
                        send_packet(sock_fd, message, sizeof(struct myftph)+st.st_size-send_length);
                        send_length = st.st_size;
                    }
                }
            } else {
                message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, 0, NULL);
                send_packet(sock_fd, message, sizeof(struct myftph));
            }
        } else {
            fprintf(stderr, "unexpected code\n");
        }
    } else {
        print_error(rmsg);
    }
    close(file_fd);
    free(message);
}

void help_cmd(int argc, char *argv[])
{
    printf("\n-- help messages --\n\n");
    printf("    quit : disconnect and quit program.\n");
    printf("    pwd  : print the current working directory in the server.\n");
    printf("    cd   : change the current working directory in the server.\n");
    printf("    dir  : list directory contents in the server.\n");
    printf("    lpwd : print the current working directory in the client.\n");
    printf("    lcd  : change the current working directory in the client.\n");
    printf("    ldir : list directory contents in the client.\n");
    printf("    get  : get a file to the server.\n");
    printf("    put  : send a file to the server.\n");
    printf("    help : print help messages.\n");
    printf("\n-- end --\n");
}

void commandline(int *argc, char *argv[])
{
    char lbuf[LINE_LENGTH];

    printf("myFTP%% ");
    if (fgets(lbuf, LINE_LENGTH, stdin) == NULL) {
        return;
    }
    getargs(lbuf, argc, argv);
}

void free_argv(int argc, char *argv[])
{
    int i;
    
    for (i = 0; i < argc; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
}

void print_error(struct myftph_data msg)
{
    switch (msg.type) {
        case MYFTPH_TYPE_ERR_CMD:
            switch (msg.code) {
                case MYFPTH_CODE_ERR_CMD_PROTOCOL:
                    fprintf(stderr, "command protocol error\n");
                    break;
                case MYFPTH_CODE_ERR_CMD_SYNTAX:
                    fprintf(stderr, "command syntax error\n");
                    break;
                case MYFPTH_CODE_ERR_CMD_UNDEFINED:
                    fprintf(stderr, "command undefined error\n");
                    break;
                default:
                    fprintf(stderr, "unknown code\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_ERR_FILE:
            switch (msg.code) {
                case MYFPTH_CODE_ERR_FILE_ACCESS:
                    fprintf(stderr, "file access error\n");
                    break;
                case MYFPTH_CODE_ERR_FILE_NOTFOUND:
                    fprintf(stderr, "file not found error\n");
                    break;
                default:
                    fprintf(stderr, "unknown code\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_ERR_UNKNOWN:
            fprintf(stderr, "unknown error\n");
            switch (msg.code) {
                case MYFPTH_CODE_ERR_UNKWOUN:
                    break;
                default:
                    fprintf(stderr, "unknown code\n");
                    break;
            }
            break;
        default:
            fprintf(stderr, "unknown type\n");
            break;
    }
}