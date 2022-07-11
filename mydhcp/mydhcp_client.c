#include "mydhcp.h"

struct proctable_client ptab_client[] = {
    {CLIENT_STATE_WAIT_OFFER, CLIENT_EVENT_RECEIVE_OFFER_OK, mydhcp_client_send_request},
    {CLIENT_STATE_WAIT_OFFER, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_send_discover},
    {CLIENT_STATE_WAIT_ACK, CLIENT_EVENT_RECEIVE_ACK_OK, mydhcp_client_set_ip},
    {CLIENT_STATE_WAIT_ACK, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_send_request},
    {CLIENT_STATE_IN_USE, CLIENT_EVENT_RECEIVE_SIGHUP, mydhcp_client_send_release},
    {CLIENT_STATE_IN_USE, CLIENT_EVENT_TTL_HALF_PASSED, mydhcp_client_send_request},
    {CLIENT_STATE_OFFER_TIMEOUT, CLIENT_EVENT_RECEIVE_OFFER_OK, mydhcp_client_send_request},
    {CLIENT_STATE_OFFER_TIMEOUT, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_exit},
    {CLIENT_STATE_ACK_TIMEOUT, CLIENT_EVENT_RECEIVE_ACK_OK, mydhcp_client_set_ip},
    {CLIENT_STATE_ACK_TIMEOUT, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_exit},
    {CLIENT_STATE_WAIT_ACK_EXTEND, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_send_request},
    {CLIENT_STATE_WAIT_ACK_EXTEND, CLIENT_EVENT_RECEIVE_ACK_OK, mydhcp_client_set_ip},
    {CLIENT_STATE_ACK_EXTEND_TIMEOUT, CLIENT_EVENT_RECEIVE_ACK_OK, mydhcp_client_set_ip},
    {CLIENT_STATE_ACK_EXTEND_TIMEOUT, CLIENT_EVENT_RECEIVE_TIMEOUT, mydhcp_client_exit}
};

struct client info;
int sock_fd;
struct sockaddr_in skt;
int client_timecount = 0;
int hup_flag = 0;

void alrm_handler()
{
    client_timecount += TIMER_INTERVAL;
}

void hup_handler()
{
    hup_flag = 1;
}

int main(int argc, char *argv[])
{
    char server_address[INET_ADDRSTRLEN];
    struct proctable_client *pt;
    struct event_table et;
    struct sockaddr_in myskt;
    struct sigaction alrm_action;
    struct sigaction hup_action;
    struct itimerval timer;

    info.status = CLIENT_STATE_INIT;
    if (argc < 2) {
        fprintf(stderr, "input server-IP-address\n");
        exit(1);
    } else if (argc == 2) {
        memcpy(server_address, argv[1], sizeof(server_address));
    } else {
        fprintf(stderr, "too many arguments\n");
        exit(1);
    }

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL)) < 0) {
        perror("socket");
        exit(1);
    }
    memset(&skt, 0, sizeof(skt));
    skt.sin_family = AF_INET;
    skt.sin_port = htons(SERVER_PORT);
    skt.sin_addr.s_addr = inet_addr(server_address);

    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(CLIENT_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }
    if (sigemptyset(&alrm_action.sa_mask) < 0) {
        perror("sigemptyset");
        exit(1);
    }
    if (sigemptyset(&alrm_action.sa_mask) < 0) {
        perror("sigemptyset");
        exit(1);
    }

    if (sigemptyset(&alrm_action.sa_mask) < 0) {
        perror("sigemptyset");
        exit(1);
    }
    if (sigemptyset(&hup_action.sa_mask) < 0) {
        perror("sigemptyset");
        exit(1);
    }
    alrm_action.sa_handler = alrm_handler;
    hup_action.sa_handler = hup_handler;

    if(sigaction(SIGALRM, &alrm_action, NULL) < 0){
        perror("sigaction alrm error");
        exit(1);
    }
    if(sigaction(SIGHUP, &hup_action, NULL) < 0){
        perror("sigaction hup error");
        exit(1);
    }
    timer.it_value.tv_sec = TIMER_INTERVAL;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TIMER_INTERVAL;
    timer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("settimer");
        exit(1);
    }

    printf("[INITIALIZE]\n\n");

    for (;;) {
        if (info.status == CLIENT_STATE_INIT) {
            mydhcp_client_send_discover(et);
        }
        et = wait_client_event();
        if (et.event != PASS_EVENT) {
            for (pt = ptab_client; pt->status; pt++) {
                if (pt->status == info.status &&
                    pt->event == et.event) {
                        (*pt->func)(et);
                        break;
                    }
            }
            if (pt->status == 0) {
                if (et.event == PACKET_ERROR) {
                    fprintf(stderr, "\n ** packet error **\n");
                    exit(1);
                } else {
                    fprintf(stderr, "\n** unexpected packet **\n");
                    exit(1);
                }
            }
        }
    }
    if (close(sock_fd) < 0) {
        perror("close");
        exit(1);
    }
    return 0;
}