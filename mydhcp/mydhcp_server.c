#include "mydhcp.h"
// int s;
// struct sockaddr_in myskt;
// fd_set rdfds;

// if ((s = socket()) < 0) {
//     // エラー処理
// }

// if (bind(s, ) < 0) {
//     // エラー処理
// }

// FD_ZERO(&rdfds);
// FD_SET(0, &rdfds);
// FD_SET(s, &rdfds);

// if (select(s+1, &rdfds, NULL, NULL, NULL) < 0) {
//     // エラー処理
// }
// if (FD_ISSET(0, &rdfds)) {
//     // 標準入力から入力
// }
// if (FD_ISSET(s, &rdfds)) {
//     // パケット受信
// }

struct proctable_server ptab_server[] = {
    {SERVER_STATE_INIT, SERVER_EVENT_RECEIVE_DISCOVER, mydhcp_server_offer},
    {SERVER_STATE_WAIT_REQUEST, SERVER_EVENT_RECEIVE_REQUEST_ALLOC, mydhcp_server_ack},
    {SERVER_STATE_IN_USE, SERVER_EVENT_RELEASE, mydhcp_server_release}
};

int main()
{
    struct proctable_server *pt;
    struct sockaddr_in skt;
    struct sockaddr_in myskt;
    struct client *client_list;
    struct event_table et;
    client_list = (struct client *)malloc(sizeof(struct client));
    client_list->status = SERVER_STATE_INIT;
    client_list->num = 1;

    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL)) < 0) {
        perror("socket");
        exit(1);
    }
    memset(&skt, 0, sizeof(skt));
    skt.sin_family = AF_INET;
    skt.sin_port = htons(CLIENT_PORT);
    skt.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(SERVER_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }

    socklen_t sktlen = sizeof(struct sockaddr);
    write(1, "main wait\n", 10);
    struct mydhcp_message message;
    int count;
    if ((count = recvfrom(fd, &message, sizeof(message), RECVFROM_FLAG, (struct sockaddr *)&skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    write(1, "receive", 8);
    print_message(message);
    close(fd);
    exit(0);

    for (;;) {
        et = wait_server_event(client_list, fd, &myskt);
        for (pt = ptab_server; pt->status; pt++) {
            if (pt->status == et.client->status &&
                pt->event == et.event) {
                    (*pt->func)(et.client, fd, &skt);
                    break;
                }
        }
        if (pt->status == 0) {
            // エラー処理
            
        }
    }
    free(client_list);
    return 0;
}