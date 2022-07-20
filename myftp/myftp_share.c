#include "myftp.h"

struct myftph_data *create_message(
    uint8_t type,
    uint8_t code,
    uint16_t length,
    char data[DATASIZE]
)
{
    struct myftph_data *message = (struct myftph_data *)malloc(sizeof(struct myftph_data));

    message->type = type;
    message->code = code;
    message->length = length;
    memcpy(message->data, data, length);

    return message;
}

void print_message(struct myftph_data *message)
{
    int i;
    switch (message->type) {
        case MYFTPH_TYPE_QUIT:
            printf("    type -> QUIT\n");
            break;
        case MYFTPH_TYPE_PWD:
            printf("    type -> PWD\n");
            break;
        case MYFTPH_TYPE_CWD:
            printf("    type -> CWD\n");
            break;
        case MYFTPH_TYPE_LIST:
            printf("    type -> LIST\n");
            break;
        case MYFTPH_TYPE_RETR:
            printf("    type -> RETR\n");
            break;
        case MYFTPH_TYPE_STOR:
            printf("    type -> STOR\n");
            break;
        case MYFTPH_TYPE_OK:
            printf("    type -> OK\n");
            switch (message->code) {
                case MYFTPH_CODE_OK_NORMAL:
                    printf("    code -> OK\n");
                    break;
                case MYFTPH_CODE_OK_SC:
                    printf("    code -> OK (server -> client)\n");
                    break;
                case MYFTPH_CODE_OK_CS:
                    printf("    code -> OK (client -> server)\n");
                    break;
                default:
                    printf("    code -> UNKNOWN CODE\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_ERR_CMD:
            printf("    type -> ERR CMD\n");
            switch (message->code) {
                case MYFPTH_CODE_ERR_CMD_SYNTAX:
                    printf("    code -> ERROR (cmd syntax error)\n");
                    break;
                case MYFPTH_CODE_ERR_CMD_UNDEFINED:
                    printf("    code -> ERROR (cmd undefined)\n");
                    break;
                case MYFPTH_CODE_ERR_CMD_PROTOCOL:
                    printf("    code -> ERROR (cmd protocol error)\n");
                    break;
                default:
                    printf("    code -> UNKNOWN CODE\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_ERR_FILE:
            printf("    type -> ERR FILE\n");
            switch (message->code) {
                case MYFPTH_CODE_ERR_FILE_NOTFOUND:
                    printf("    code -> ERROR (file/directory not found)\n");
                    break;
                case MYFPTH_CODE_ERR_FILE_ACCESS:
                    printf("    code -> ERROR (no permission to access file/directory)\n");
                    break;
                default:
                    printf("    code -> UNKNOWN CODE\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_ERR_UNKNOWN:
            printf("    type -> ERR UNKNOWN\n");
            switch (message->code) {
                case MYFPTH_CODE_ERR_UNKWOUN:
                    printf("    code -> ERROR (unknown error)\n");
                    break;
                default:
                    printf("    code -> UNKNOWN CODE\n");
                    break;
            }
            break;
        case MYFTPH_TYPE_DATA:
            printf("    type -> DATA\n");
            switch (message->code) {
                case MYFTPH_CODE_DATA_FINISH:
                    printf("    code -> DATA FINISH\n");
                    break;
                case MYFTPH_CODE_DATA_CONTINUE:
                    printf("    code -> DATA CONTINUE\n");
                    break;
                default:
                    printf("    code -> UNKNOWN CODE\n");
                    break;
            }
            break;
        default:
            printf("    type -> UNKNOWN TYPE\n");
            break;
    }
    printf("    length -> %d\n", message->length);
    if (message->length > 0) {
        if (message->length > 128) {
            printf("    data -> XXX\n");
        } else {
            printf("    data -> ");
            //print_data(*message);
            printf("\n");
        }
    }
}

void print_data(struct myftph_data msg)
{
    int i;
    for (i=0;i<msg.length;i++) {
        printf("%c", msg.data[i]);
    }
}

void send_packet(int fd, struct myftph_data *msg, int msg_size)
{
    if (send(fd, msg, msg_size, 0) < 0) {
        perror("send");
        exit(1);
    }
    // printf("\n-- send packet --\n");
    // print_message(msg);
    // printf("-- end --\n");
}

void receive_packet(int fd, struct myftph_data *msg)
{
    if(recv(fd, msg, sizeof(struct myftph), 0) < 0) {
        perror("recv");
        exit(1);
    }
    if (msg->length > 0) {
        char *data = (char *)malloc(sizeof(char)*msg->length);
        if(recv(fd, data, msg->length, 0) < 0) {
            perror("recv");
            exit(1);
        }
        memcpy(msg->data, data, msg->length);
        free(data);
    }
    // printf("\n-- receive packet --\n");
    // print_message(msg);
    // printf("-- end --\n");
}

void set_permission(char *buf, char *path)
{
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        strncat(buf, "d", 1);
    } else if (S_ISLNK(st.st_mode)) {
        strncat(buf, "|", 1);
    } else if (S_ISREG(st.st_mode)) {
        strncat(buf, "-", 1);
    } else if (S_ISSOCK(st.st_mode)) {
        strncat(buf, "s", 1);
    } else if (S_ISBLK(st.st_mode)) {
        strncat(buf, "b", 1);
    } else if (S_ISSOCK(st.st_mode)) {
        strncat(buf, "c", 1);
    } else if (S_ISCHR(st.st_mode)) {
        strncat(buf, "c", 1);
    } else if (S_ISFIFO(st.st_mode)) {
        strncat(buf, "p", 1);
    }

    if (st.st_mode & S_IRUSR) {
        strncat(buf, "r", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IWUSR) {
        strncat(buf, "w", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IXUSR) {
        strncat(buf, "x", 1);
    } else {
        strncat(buf, "-", 1);
    }

    if (st.st_mode & S_IRGRP) {
        strncat(buf, "r", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IWGRP) {
        strncat(buf, "w", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IXGRP) {
        strncat(buf, "x", 1);
    } else {
        strncat(buf, "-", 1);
    }

    if (st.st_mode & S_IROTH) {
        strncat(buf, "r", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IWOTH) {
        strncat(buf, "w", 1);
    } else {
        strncat(buf, "-", 1);
    }
    if (st.st_mode & S_IXOTH) {
        strncat(buf, "x", 1);
    } else {
        strncat(buf, "-", 1);
    }
    strncat(buf, " ", 1);
}

int myls(char *path, char *buf)
{
    struct stat st;
    DIR *dp;
    struct dirent *dir;
    
    if (stat(path, &st) < 0) {
        perror("stat");
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
         if ((dp = opendir(path)) == NULL) {
            perror("opendir");
            return -2;
        }
        while ((dir = readdir(dp)) != NULL) {
            if (dir->d_ino == 0) {
                continue;
            }
            if (strcmp(dir->d_name, ".") == 0) {
                continue;
            } else if (strcmp(dir->d_name, "..") == 0) {
                continue;
            }
            set_permission(buf, dir->d_name);
            strncat(buf, dir->d_name, strlen(dir->d_name));
            strncat(buf, "\n", 1);
        }
        if (closedir(dp) < 0) {
            perror("closedir");
            return -3;
        }
    } else {
        strncat(buf, path, strlen(path));
        strncat(buf, "\n", 1);
    }
    return 0;
}