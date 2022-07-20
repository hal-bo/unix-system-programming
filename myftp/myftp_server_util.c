#include "myftp.h"

void server_quit_proc(int fd, struct myftph_data msg)
{
    struct myftph_data *message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_NORMAL, 0, NULL);
    send_packet(fd, message, sizeof(struct myftph));
    printf("disconnect\n");
    close(fd);
    exit(0);
}

void server_pwd_proc(int fd, struct myftph_data msg)
{
    char wd[DATASIZE];
    getcwd(wd, DATASIZE);
    int length = strlen(wd);
    struct myftph_data *message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_NORMAL, length, wd);
    send_packet(fd, message, sizeof(struct myftph) + length);
    free(message);
}

void server_cd_proc(int fd, struct myftph_data msg)
{
    struct myftph_data *message;
    char *path = (char *)malloc(sizeof(char) * (msg.length+1));
    memcpy(path, msg.data, msg.length+1);
    path[msg.length] = '\0';
    if (chdir(path) < 0) {
        perror("lcd");
        if (errno == ENOENT) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_NOTFOUND, 0, NULL);
        } else if (errno == EACCES) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_ACCESS, 0, NULL);
        } else {
            message = create_message(MYFTPH_TYPE_ERR_UNKNOWN, MYFPTH_CODE_ERR_UNKWOUN, 0, NULL);
        }
    } else {
        message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_NORMAL, 0, NULL);
    }
    send_packet(fd, message, sizeof(struct myftph));
    free(message);
}

void server_dir_proc(int fd, struct myftph_data msg)
{
    struct myftph_data *message;
    char buf[BUFSIZE];
    int status;
    char *path;
    if (msg.length > 0) {
        path = (char *)malloc(sizeof(char) * (msg.length+1));
        memcpy(path, msg.data, msg.length+1);
        path[msg.length] = '\0';
    } else {
        path = ".";
    }
    memcpy(buf, "", BUFSIZE);
    if ((status = myls(path, buf)) < 0) {
        if (status == -1) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_NOTFOUND, 0, NULL);
        } else if (status == -2) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_ACCESS, 0, NULL);
        } else {
            message = create_message(MYFTPH_TYPE_ERR_UNKNOWN, MYFPTH_CODE_ERR_UNKWOUN, 0, NULL);
        }
        send_packet(fd, message, sizeof(struct myftph));
    } else {
        message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_NORMAL, 0, NULL);
        send_packet(fd, message, sizeof(struct myftph));
        int length = strlen(buf);
        if (length > 0) {
            int send_length = 0;
            while (length > send_length) {
                if (length - send_length > DATASIZE) {
                    message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_CONTINUE, DATASIZE, buf+send_length);
                    send_packet(fd, message, sizeof(struct myftph)+DATASIZE);
                    send_length += DATASIZE;
                } else {
                    message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, length-send_length, buf);
                    send_packet(fd, message, sizeof(struct myftph)+length-send_length);
                    send_length = length;
                }
            }
        } else {
            message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, 1, "\n");
            send_packet(fd, message, sizeof(struct myftph)+1);
        }
    }
    free(message);
    if (msg.length > 0) {
        free(path);
    }
}

void server_get_proc(int fd, struct myftph_data msg)
{
    struct myftph_data *message;
    char buf[BUFSIZE];
    struct stat st;
    int send_length = 0;
    int file_fd, n;
    char *path = (char *)malloc(sizeof(char) * (msg.length+1));
    memcpy(buf, "", BUFSIZE);
    memcpy(path, msg.data, msg.length+1);
    path[msg.length] = '\0';

    if (stat(path, &st) < 0) {
        perror("stat");
        if (errno == ENOENT) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_NOTFOUND, 0, NULL);
        } else if (errno == EACCES) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_ACCESS, 0, NULL);
        } else {
            message = create_message(MYFTPH_TYPE_ERR_UNKNOWN, MYFPTH_CODE_ERR_UNKWOUN, 0, NULL);
        }
        send_packet(fd, message, sizeof(struct myftph));
        return;
    }
    if ((file_fd = open(path, O_RDONLY)) < 0) {
        perror(path);
        if (errno == ENOENT) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_NOTFOUND, 0, NULL);
        } else if (errno == EACCES) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_ACCESS, 0, NULL);
        } else {
            message = create_message(MYFTPH_TYPE_ERR_UNKNOWN, MYFPTH_CODE_ERR_UNKWOUN, 0, NULL);
        }
        send_packet(fd, message, sizeof(struct myftph));
        return;
    }
    message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_SC, 0, NULL);
    send_packet(fd, message, sizeof(struct myftph));
    if (st.st_size > 0) {
        while (st.st_size > send_length) {
            if (st.st_size - send_length > DATASIZE) {
                if ((n = read(file_fd, buf, DATASIZE)) < 0) {
                    perror("read");
                    close(file_fd);
                    return;
                }
                message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_CONTINUE, DATASIZE, buf);
                send_packet(fd, message, sizeof(struct myftph)+DATASIZE);
                send_length += DATASIZE;
            } else {
                if ((n = read(file_fd, buf, st.st_size - send_length)) < 0) {
                    perror("read");
                    close(file_fd);
                    return;
                }
                message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, st.st_size-send_length, buf);
                send_packet(fd, message, sizeof(struct myftph)+st.st_size-send_length);
                send_length = st.st_size;
            }
        }
    } else {
        message = create_message(MYFTPH_TYPE_DATA, MYFTPH_CODE_DATA_FINISH, 0, NULL);
        send_packet(fd, message, sizeof(struct myftph));
    }
    close(file_fd);
    free(message);
}

void server_put_proc(int fd, struct myftph_data msg)
{
    struct myftph_data *message;
    char buf[BUFSIZE];
    struct myftph_data rmsg;
    int status;
    FILE *fp;
    int file_fd;
    int buf_length = 0;
    char *path = (char *)malloc(sizeof(char) * (msg.length+1));
    memcpy(buf, "", BUFSIZE);
    memcpy(path, msg.data, msg.length+1);
    path[msg.length] = '\0';

    if ((file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror(path);
        if (errno == ENOENT) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_NOTFOUND, 0, NULL);
        } else if (errno == EACCES) {
            message = create_message(MYFTPH_TYPE_ERR_FILE, MYFPTH_CODE_ERR_FILE_ACCESS, 0, NULL);
        } else {
            message = create_message(MYFTPH_TYPE_ERR_UNKNOWN, MYFPTH_CODE_ERR_UNKWOUN, 0, NULL);
        }
        send_packet(fd, message, sizeof(struct myftph));
        return;
    } else {
        message = create_message(MYFTPH_TYPE_OK, MYFTPH_CODE_OK_CS, 0, NULL);
        send_packet(fd, message, sizeof(struct myftph));
    }
    receive_packet(fd, &rmsg);
    while (rmsg.code == MYFTPH_CODE_DATA_CONTINUE) {
        memcpy(buf+buf_length, rmsg.data, rmsg.length);
        buf_length += rmsg.length;
        receive_packet(fd, &rmsg);
    }
    if (rmsg.code == MYFTPH_CODE_DATA_FINISH) {
        memcpy(buf+buf_length, rmsg.data, rmsg.length);
        if (write(file_fd, buf, buf_length+rmsg.length) < 0) {
            perror("write");
        }
    } else {
        fprintf(stderr, "invalid message\n");
    }
    fclose(fp);
    free(message);
}
