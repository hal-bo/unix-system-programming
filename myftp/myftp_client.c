#include "myftp.h"

int main(int argc, char** argv)
{
    int sock_fd;
    struct sockaddr_in myskt;
    struct addrinfo *server_addr = NULL;

    if (argc < 2) {
        fprintf(stderr, "input server-host-name\n");
        exit(1);
    } else if (argc == 2) {
        set_addrinfo(argv[1], server_addr);
    } else {
        fprintf(stderr, "too many arguments\n");
        exit(1);
    }
 
    // IPv4 TCP のソケットを作成する
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
 
    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(CLIENT_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }

    char output[256];
    getnameinfo(server_addr->ai_addr, server_addr->ai_addrlen, output, sizeof(output), NULL, 0, NI_NUMERICHOST);
    printf("\n-- server addr --\n");
    printf("    ip       -> %s\n", output);
    printf("-- end --\n");
    // サーバ接続（TCP の場合は、接続を確立する必要がある）
    connect(sock_fd, server_addr->ai_addr, server_addr->ai_addrlen);
 
    // パケットを TCP で送信
    if(send(sock_fd, "I am send process", 17, 0) < 0) {
        perror("send");
        return -1;
    }
 
    close(sock_fd);
 
    return 0;
}