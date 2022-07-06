#include "mydhcp.h"

void mydhcp_server_offer(struct client *c, int fd, struct sockaddr_in *skt)
{
    printf("mydhcp_server_offer\n");
    set_server_status(c, SERVER_STATE_WAIT_REQUEST);
}

void mydhcp_server_ack(struct client *c, int fd, struct sockaddr_in *skt)
{
    printf("mydhcp_server_ack\n");
    set_server_status(c, SERVER_STATE_IN_USE);
}

void mydhcp_server_release(struct client *c, int fd, struct sockaddr_in *skt)
{
    printf("mydhcp_server_release\n");
    set_server_status(c, SERVER_STATE_TERMINATE);
}

void set_server_status(struct client *c, int new_status)
{
    printf("\n[ ");
    switch (c->status) {
        case SERVER_STATE_INIT:
            printf("INIT -> ");
            break;
        case SERVER_STATE_WAIT_REQUEST:
            printf("WAIT_REQUEST -> ");
            break;
        case SERVER_STATE_IN_USE:
            printf("IN_USE -> ");
            break;
    }
    switch (new_status) {
        case SERVER_STATE_INIT:
            printf("INIT");
            break;
        case SERVER_STATE_WAIT_REQUEST:
            printf("WAIT_REQUEST");
            break;
        case SERVER_STATE_IN_USE:
            printf("IN_USE");
            break;
        case SERVER_STATE_TERMINATE:
            printf("TERMINATE");
            break;
    }
    c->status = new_status;
    printf(" ]\n\n");
}

struct event_table wait_server_event(struct client *client_list, int fd, struct sockaddr_in *myskt)
{
    int count;
    struct mydhcp_message message;
    socklen_t sktlen = sizeof(struct sockaddr);
    if ((count = recvfrom(fd, &message, sizeof(message), RECVFROM_FLAG, (struct sockaddr *)myskt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("recvfrom\n");
    print_message(message);
    close(fd);
    exit(0);
    struct event_table et;
    et.client = client_list; // debug
    printf("EVENT: ");
    switch (et.client->status) {
        case SERVER_STATE_INIT:
            printf("SERVER_EVENT_RECEIVE_DISCOVER\n");
            et.event = SERVER_EVENT_RECEIVE_DISCOVER;
            break;
        case SERVER_STATE_WAIT_REQUEST:
            printf("SERVER_EVENT_RECEIVE_REQUEST_ALLOC\n");
            et.event = SERVER_EVENT_RECEIVE_REQUEST_ALLOC;
            break;
        case SERVER_STATE_IN_USE:
            printf("SERVER_EVENT_RELEASE\n");
            et.event = SERVER_EVENT_RELEASE;
            break;
        case SERVER_STATE_TERMINATE:
            printf("EXIT\n\n");
            exit(0);
            break;
    }
    return et;
}

// void add_client(struct client *c, struct client *list)
// {
//     p->fp = h;
//     p->bp = h->bp;
//     h->bp->
// }

// void remove_client(struct client *list)
// {
//     list->bp->fp = 
// }

void print_event_table(struct event_table et)
{
    printf("event -> ");
    switch (et.event) {
        case SERVER_EVENT_RECEIVE_DISCOVER:
            printf("SERVER_EVENT_RECEIVE_DISCOVER\n");
            break;
        case SERVER_EVENT_RECEIVE_REQUEST_ALLOC:
            printf("SERVER_EVENT_RECEIVE_REQUEST_ALLOC\n");
            break;
        case SERVER_EVENT_RELEASE:
            printf("SERVER_EVENT_RELEASE\n");
            break;
    }
    printf("status -> ");
    switch (et.client->status) {
        case SERVER_STATE_INIT:
            printf("SERVER_STATE_INIT\n");
            break;
        case SERVER_STATE_WAIT_REQUEST:
            printf("SERVER_STATE_WAIT_REQUEST\n");
            break;
        case SERVER_STATE_IN_USE:
            printf("SERVER_STATE_IN_USE\n");
            break;
        case SERVER_STATE_TERMINATE:
            printf("SERVER_STATE_TERMINATE\n");
            exit(0);
            break;
    }
}