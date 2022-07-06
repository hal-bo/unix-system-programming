#include "mydhcp.h"

struct proctable_client ptab_client[] = {
    {CLIENT_STATE_INIT, CLIENT_EVENT_INIT, mydhcp_client_send_discover},
    {CLIENT_STATE_WAIT_OFFER, CLIENT_EVENT_RECEIVE_OFFER, mydhcp_client_send_request},
    {CLIENT_STATE_WAIT_ACK, CLIENT_EVENT_RECEIVE_ACK, mydhcp_client_set_ip},
    {CLIENT_STATE_IN_USE, CLIENT_EVENT_RECEIVE_SIGHUP, mydhcp_client_send_release}
};

int main()
{
    struct proctable_client *pt;
    int event;
    int status = CLIENT_STATE_INIT;

    struct sockaddr_in skt;
    struct sockaddr_in myskt;
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL)) < 0) {
        perror("socket");
        exit(1);
    }
    memset(&skt, 0, sizeof(skt));
    skt.sin_family = AF_INET;
    skt.sin_port = htons(SERVER_PORT);
    skt.sin_addr.s_addr = inet_addr("192.168.1.51");
    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(CLIENT_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&skt, sizeof(skt)) < 0) {
        perror("bind");
        exit(1);
    }

    for (;;) {
        event = wait_client_event(status);
        for (pt = ptab_client; pt->status; pt++) {
            if (pt->status == status &&
                pt->event == event) {
                    (*pt->func)(&status, fd, &skt);
                    break;
                }
        }
        if (pt->status == 0) {
            // エラー処理
            
        }
    }
    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }
    return 0;
}