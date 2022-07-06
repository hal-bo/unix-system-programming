#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/select.h>
#ifndef MYDHCP_H
#define MYDHCP_H

#define SOCKET_PROTOCOL 0
#define SENDTO_FLAG 0
#define RECVFROM_FLAG 0

#define SERVER_STATE_INIT 1
#define SERVER_STATE_WAIT_REQUEST 2
#define SERVER_STATE_IN_USE 3
#define SERVER_STATE_TERMINATE 4

#define CLIENT_STATE_INIT 1
#define CLIENT_STATE_WAIT_OFFER 2
#define CLIENT_STATE_WAIT_ACK 3
#define CLIENT_STATE_IN_USE 4
#define CLIENT_STATE_EXIT 5

#define SERVER_EVENT_RECEIVE_DISCOVER 1
#define SERVER_EVENT_RECEIVE_REQUEST_ALLOC 2
#define SERVER_EVENT_RECEIVE_REQUEST_EXTEND 3
#define SERVER_EVENT_RELEASE 4
#define SERVER_EVENT_RECEIVE_RELEASE 5

#define CLIENT_EVENT_INIT 1
#define CLIENT_EVENT_RECEIVE_OFFER 2
#define CLIENT_EVENT_RECEIVE_ACK 3
#define CLIENT_EVENT_RECEIVE_SIGHUP 4

#define SERVER_PORT 51230
#define CLIENT_PORT 51231

#define TYPE_DISCOVER 1
#define TYPE_OFFER 2
#define TYPE_REQUEST 3
#define TYPE_ACK 4
#define TYPE_RELEASE 5

#define OFFER_CODE_OK 0 // 正常
#define OFFER_CODE_NG 1 // エラー
#define REQUEST_CODE_ALLOCK 2 // 割り当て要求
#define REQUEST_CODE_EXTEND 3 // 使用期間延長要求
#define ACK_CODE_OK 0 // 正常
#define ACK_CODE_NG 4 // エラー（REQUEST メッセージに誤りがあった）

#define IGNORE 0

struct client {
    int num; // デバッグ用
    struct client *fp;
    struct client *bp;
    int status;
    int ttlcounter;

    struct in_addr id;
    struct in_addr addr;
    struct in_addr netmask;
    uint16_t ttl;
};

struct proctable_server {
    int status;
    int event;
    void (*func)(struct client *c, int fd, struct sockaddr_in *skt);
};
struct proctable_client {
    int status;
    int event;
    void (*func)(int *current_status, int fd, struct sockaddr_in *skt);
};

struct mydhcp_message {
    uint8_t type;
    uint8_t code;
    uint16_t ttl;
    in_addr_t address;
    in_addr_t netmask;
};

struct event_table {
    int event;
    struct client *client;
};

void mydhcp_server_offer(struct client *c, int fd, struct sockaddr_in *skt);
void mydhcp_server_ack(struct client *c, int fd, struct sockaddr_in *skt);
void mydhcp_server_release(struct client *c, int fd, struct sockaddr_in *skt);
void set_server_status(struct client *c, int new_status);
struct event_table wait_server_event(struct client *client_list, int fd, struct sockaddr_in *skt);

void mydhcp_client_send_discover(int *current_status, int s, struct sockaddr_in *skt);
void mydhcp_client_send_request(int *current_status, int s, struct sockaddr_in *skt);
void mydhcp_client_set_ip(int *current_status, int s, struct sockaddr_in *skt);
void mydhcp_client_send_release(int *current_status, int s, struct sockaddr_in *skt);
void set_client_status(int *current_status, int new_status);
int wait_client_event(int client_status);

struct mydhcp_message create_message(
    uint8_t type,
    uint8_t code,
    uint16_t ttl,
    in_addr_t address,
    in_addr_t netmask
);
void print_message(struct mydhcp_message message);
#endif