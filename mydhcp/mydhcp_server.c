#include "mydhcp.h"

struct proctable_server ptab_server[] = {
    {SERVER_STATE_INIT, SERVER_EVENT_RECEIVE_DISCOVER, mydhcp_server_offer},
    {SERVER_STATE_WAIT_REQUEST, SERVER_EVENT_RECEIVE_REQUEST_ALLOC, mydhcp_server_ack},
    {SERVER_STATE_WAIT_REQUEST, SERVER_EVENT_REQUEST_ERROR, mydhcp_server_release},
    {SERVER_STATE_WAIT_REQUEST, SERVER_EVENT_RECEIVE_TIMEOUT, mydhcp_server_offer},
    {SERVER_STATE_IN_USE, SERVER_EVENT_RECEIVE_REQUEST_EXTEND, mydhcp_server_ack},
    {SERVER_STATE_IN_USE, SERVER_EVENT_RECEIVE_RELEASE, mydhcp_server_release},
    {SERVER_STATE_IN_USE, SERVER_EVENT_REQUEST_ERROR, mydhcp_server_terminate},
    {SERVER_STATE_REQUEST_TIMEOUT, SERVER_EVENT_RECEIVE_REQUEST_ALLOC, mydhcp_server_ack},
    {SERVER_STATE_REQUEST_TIMEOUT, SERVER_EVENT_RECEIVE_TIMEOUT, mydhcp_server_release},
    {SERVER_STATE_REQUEST_TIMEOUT, SERVER_EVENT_REQUEST_ERROR, mydhcp_server_release}
};

struct ip_info *ip_list_head;
int list_size;
int default_ttl_time;
struct client *client_list_head;
int sock_fd;
int server_timecount = 0;

void alrm_handler()
{
    server_timecount += TIMER_INTERVAL;
}

int main(int argc, char *argv[])
{
    struct proctable_server *pt;
    struct sockaddr_in myskt;
    struct event_table et;
    struct sigaction alrm_action;
    struct itimerval timer;

    client_list_head = (struct client *)malloc(sizeof(struct client));
    client_list_head->fp = client_list_head;
    client_list_head->bp = client_list_head;
    ip_list_head = (struct ip_info *)malloc(sizeof(struct ip_info));
    ip_list_head->fp = ip_list_head;
    ip_list_head->bp = ip_list_head;
    print_client();

    if (argc < 2) {
        fprintf(stderr, "input config-file\n");
        exit(1);
    } else if (argc == 2) {
        read_config(argv[1]);
    } else {
        fprintf(stderr, "too many arguments\n");
        exit(1);
    }

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, SOCKET_PROTOCOL)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&myskt, 0, sizeof(myskt));
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(SERVER_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
        perror("bind");
        exit(1);
    }

    if (sigemptyset(&alrm_action.sa_mask) < 0) {
        perror("sigemptyset");
        exit(1);
    }
    alrm_action.sa_handler = alrm_handler;
    if (sigaction(SIGALRM, &alrm_action, NULL) < 0) {
        perror("sigaction");
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
        if ((et = wait_server_event(&myskt)).event != PASS_EVENT) {
            print_event_table(et);
            if (et.event == EVENT_NG) {
                fprintf(stderr, "\n** unexpected packet **\n");
            } else {
                for (pt = ptab_server; pt->status; pt++) {
                    if (pt->status == et.client->status &&
                        pt->event == et.event) {
                            (*pt->func)(et);
                            break;
                        }
                }
                if (pt->status == 0) {
                    if (et.event == PACKET_ERROR) {
                        fprintf(stderr, "\n ** packet error **\n");
                    }
                    fprintf(stderr, "??\n");
                }
            }
        }
    
    }
    free(client_list_head);
    close(sock_fd);
    return 0;
}