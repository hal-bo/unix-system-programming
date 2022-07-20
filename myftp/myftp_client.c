#include "myftp.h"

struct myftp_cmd {
    char *cmd;
    void (*func)(int, char *[]);
}
cmd_tbl[] = {
    {"quit", quit_cmd},
    {"pwd", pwd_cmd},
    {"cd", cd_cmd},
    {"dir", dir_cmd},
    {"lpwd", lpwd_cmd},
    {"lcd", lcd_cmd},
    {"ldir", ldir_cmd},
    {"get", get_cmd},
    {"put", put_cmd},
    {"help", help_cmd},
    {NULL, NULL}
};

int sock_fd;

int main(int argc, char** argv)
{
    struct sockaddr_in skt;
    struct sockaddr_in myskt;
    struct hostent *hp;
    struct myftp_cmd *p;
    int ac;
    char *av[COMMAND_SIZE];

    if (argc < 2) {
        fprintf(stderr, "input server-host-name\n");
        exit(1);
    } else if (argc == 2) {
        hp = gethostbyname(argv[1]);
    } else {
        fprintf(stderr, "too many arguments\n");
        exit(1);
    }

	if (hp == NULL || hp->h_addrtype != AF_INET || hp->h_length != 4) {
		fprintf(stderr, "Unknown host \"%s\"\n", argv[1]);
		return (-1);
	}

    // IPv4 TCP のソケットを作成する
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
 
 
    memset(&skt, 0, sizeof(skt));
    skt.sin_family = AF_INET;
    skt.sin_port = htons(SERVER_PORT);
    memcpy((void *)&skt.sin_addr, (void *)hp->h_addr, hp->h_length);

    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(CLIENT_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }

    // サーバ接続（TCP の場合は、接続を確立する必要がある）
    if (connect(sock_fd, (struct sockaddr *)&skt, sizeof(skt)) < 0) {
        printf("connect error\n");
        exit(1);
    }

    while (1) {
        commandline(&ac, av);
        if (av[0]) {
            for (p = cmd_tbl; p->cmd; p++) {
                if (strcmp(av[0], p->cmd) == 0) {
                    (*p->func)(ac, av);
                    break;
                }
            }
            if (p->cmd == NULL) {
                fprintf(stderr, "unknown command: %s\n", av[0]);
            }
            free_argv(ac, av);
        }
    }
 
    // パケットを TCP で送信
    if(send(sock_fd, "I am send process", 17, 0) < 0) {
        perror("send");
        return -1;
    }

 
    return 0;
}