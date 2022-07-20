#include "myftp.h"

struct myftp_server_proc {
    int proc;
    void (*func)(int, struct myftph_data);
}
proc_tbl[] = {
    {MYFTPH_TYPE_QUIT, server_quit_proc},
    {MYFTPH_TYPE_PWD, server_pwd_proc},
    {MYFTPH_TYPE_CWD, server_cd_proc},
    {MYFTPH_TYPE_LIST, server_dir_proc},
    {MYFTPH_TYPE_RETR, server_get_proc},
    {MYFTPH_TYPE_STOR, server_put_proc},
    {MYFTPH_TYPE_NULL, NULL}
};

int main(int argc, char** argv)
{
    int sock_fd;
    int acc_fd;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    struct myftph_data msg;
    int pid;
    struct myftp_server_proc *p;
 
    socklen_t sin_size = sizeof(struct sockaddr_in);
 
    char buf[2048];

    if (argc > 2) {
        argv = 0;
    }
 
   // 受信バッファの初期化
    memset(buf, 0, sizeof(buf));
 
    // IPv4 TCP のソケットを作成
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(SERVER_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);
 
    // バインドする
    if(bind(sock_fd, (struct sockaddr *)&myskt, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        return -1;
    }
 
    // パケット受信待ち状態とする
    // 待ちうけキューを１０としている
    if(listen(sock_fd, 10) < 0) {
        perror("listen");
        return -1;
    }

    while (1) {
        // クライアントからコネクト要求が来るまで停止する
        // 以降、サーバ側は acc_fd を使ってパケットの送受信を行う
        if((acc_fd = accept(sock_fd, (struct sockaddr *)&skt, &sin_size)) < 0) {
            perror("accept");
            return -1;
        }
        printf("connect\n");
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            while (1) {
                receive_packet(acc_fd, &msg);
                for (p = proc_tbl; p->proc != MYFTPH_TYPE_NULL; p++) {
                    if (p->proc == msg.type) {
                        (*p->func)(acc_fd, msg);
                        break;
                    }
                }
            }
        } else {
            close(acc_fd);
        }
    }

    close(sock_fd);
    return 0;
}