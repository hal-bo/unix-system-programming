#include "myftp.h"

int main(int argc, char** argv)
{
    int sock_fd;
    int acc_fd;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
 
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
 
    // クライアントからコネクト要求が来るまで停止する
    // 以降、サーバ側は acc_fd を使ってパケットの送受信を行う
    if((acc_fd = accept(sock_fd, (struct sockaddr *)&skt, &sin_size)) < 0) {
        perror("accept");
        return -1;
    }
  
    // パケット受信。パケットが到着するまでブロック
    if(recv(acc_fd, buf, sizeof(buf), 0) < 0) {
        perror("recv");
        return -1;
    }
 
    // パケット送受信用ソケットのクローズ
    close(acc_fd);
 
    // 接続要求待ち受け用ソケットをクローズ
    close(sock_fd);
 
    // 受信データの出力
    printf("%s\n", buf);
 
    return 0;
}