#include "mydhcp.h"

void mydhcp_client_send_discover(int *current_status, int fd, struct sockaddr_in *skt)
{
    struct mydhcp_message message;
    int count;

    printf("mydhcp_client_send_discover\n");
    message = create_message(TYPE_DISCOVER, IGNORE, IGNORE, IGNORE, IGNORE);
    if ((count = sendto(fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    print_message(message);
    close(fd);
    exit(0);
    set_client_status(current_status, CLIENT_STATE_WAIT_OFFER);
}

void mydhcp_client_send_request(int *current_status, int s, struct sockaddr_in *skt)
{
    printf("mydhcp_client_send_request\n");
    set_client_status(current_status, CLIENT_STATE_WAIT_ACK);
}

void mydhcp_client_set_ip(int *current_status, int s, struct sockaddr_in *skt)
{
    printf("mydhcp_client_set_ip\n");
    set_client_status(current_status, CLIENT_STATE_IN_USE);
}

void mydhcp_client_send_release(int *current_status, int s, struct sockaddr_in *skt)
{
    printf("mydhcp_client_send_release\n");
    set_client_status(current_status, CLIENT_STATE_EXIT);
}

void set_client_status(int *current_status, int new_status)
{
    printf("\n[ ");
    switch (*current_status) {
        case CLIENT_STATE_INIT:
            printf("INIT -> ");
            break;
        case CLIENT_STATE_WAIT_OFFER:
            printf("WAIT_OFFER -> ");
            break;
        case CLIENT_STATE_WAIT_ACK:
            printf("WAIT_ACK -> ");
            break;
        case CLIENT_STATE_IN_USE:
            printf("IN_USE -> ");
            break;
    }
    switch (new_status) {
        case CLIENT_STATE_INIT:
            printf("INIT");
            break;
        case CLIENT_STATE_WAIT_OFFER:
            printf("WAIT_OFFER");
            break;
        case CLIENT_STATE_WAIT_ACK:
            printf("WAIT_ACK");
            break;
        case CLIENT_STATE_IN_USE:
            printf("IN_USE");
            break;
        case CLIENT_STATE_EXIT:
            printf("EXIT");
            break;
    }
    *current_status = new_status;
    printf(" ]\n\n");
}

int wait_client_event(int client_status)
{
    printf("EVENT: ");
    switch (client_status) {
        case CLIENT_STATE_INIT:
            printf("CLIENT_EVENT_INIT\n");
            return CLIENT_EVENT_INIT;
        case CLIENT_STATE_WAIT_OFFER:
            printf("CLIENT_EVENT_RECEIVE_OFFER\n");
            return CLIENT_EVENT_RECEIVE_OFFER;
        case CLIENT_STATE_WAIT_ACK:
            printf("CLIENT_EVENT_RECEIVE_ACK\n");
            return CLIENT_EVENT_RECEIVE_ACK;
        case CLIENT_STATE_IN_USE:
            printf("CLIENT_EVENT_RECEIVE_SIGHUP\n");
            return CLIENT_EVENT_RECEIVE_SIGHUP;
        case CLIENT_STATE_EXIT:
            printf("EXIT\n\n");
            exit(0);
            break;
    }
    return CLIENT_EVENT_INIT;
}
