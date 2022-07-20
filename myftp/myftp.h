#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/select.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DATASIZE 1024
#define BUFSIZE 16384

#define LINE_LENGTH 256
#define COMMAND_SIZE 10

#define SERVER_PORT 50021
#define SERVER_PORT_STR "50021"
#define CLIENT_PORT 50024

#define MYFTPH_TYPE_QUIT 0x01
#define MYFTPH_TYPE_PWD 0x02
#define MYFTPH_TYPE_CWD 0x03
#define MYFTPH_TYPE_LIST 0x04
#define MYFTPH_TYPE_RETR 0x05
#define MYFTPH_TYPE_STOR 0x06
#define MYFTPH_TYPE_OK 0x10
#define MYFTPH_TYPE_ERR_CMD 0x11
#define MYFTPH_TYPE_ERR_FILE 0x12
#define MYFTPH_TYPE_ERR_UNKNOWN 0x13
#define MYFTPH_TYPE_DATA 0x20

#define MYFTPH_TYPE_NULL 0x99

#define MYFTPH_CODE_IGNORE 0x00
#define MYFTPH_CODE_OK_NORMAL 0x00
#define MYFTPH_CODE_OK_SC 0x01
#define MYFTPH_CODE_OK_CS 0x02
#define MYFPTH_CODE_ERR_CMD_SYNTAX 0x01
#define MYFPTH_CODE_ERR_CMD_UNDEFINED 0x02
#define MYFPTH_CODE_ERR_CMD_PROTOCOL 0x03
#define MYFPTH_CODE_ERR_FILE_NOTFOUND 0x00
#define MYFPTH_CODE_ERR_FILE_ACCESS 0x01
#define MYFPTH_CODE_ERR_UNKWOUN 0x05
#define MYFTPH_CODE_DATA_FINISH 0x00
#define MYFTPH_CODE_DATA_CONTINUE 0x01

struct myftph {
    uint8_t type;
    uint8_t code;
    uint16_t length;
};
struct myftph_data {
    uint8_t type;
    uint8_t code;
    uint16_t length;
    char data[DATASIZE];
};

struct myftph_data *create_message(uint8_t type,uint8_t code,uint16_t length,char data[DATASIZE]);
void print_message(struct myftph_data *message);
void print_data(struct myftph_data msg);
void send_packet(int fd, struct myftph_data *msg, int msg_size);
void receive_packet(int fd, struct myftph_data *msg);
int myls(char *path, char *buf);

void server_quit_proc(int fd, struct myftph_data msg);
void server_pwd_proc(int fd, struct myftph_data msg);
void server_cd_proc(int fd, struct myftph_data msg);
void server_dir_proc(int fd, struct myftph_data msg);
void server_get_proc(int fd, struct myftph_data msg);
void server_put_proc(int fd, struct myftph_data msg);

void quit_cmd(int argc, char *argv[]);
void pwd_cmd(int argc, char *argv[]);
void cd_cmd(int argc, char *argv[]);
void dir_cmd(int argc, char *argv[]);
void lpwd_cmd(int argc, char *argv[]);
void lcd_cmd(int argc, char *argv[]);
void ldir_cmd(int argc, char *argv[]);
void get_cmd(int argc, char *argv[]);
void put_cmd(int argc, char *argv[]);
void help_cmd(int argc, char *argv[]);
void commandline(int *argc, char *argv[]);
void free_argv(int argc, char *argv[]);
void print_error(struct myftph_data msg);

int is_str_numeric(char *str);
char *next_word(char *str);
int	get_word_num(char *str);
char **do_split(char **ret, char *str, int size);
char **getargs(char *str, int *argc, char *argv[]);