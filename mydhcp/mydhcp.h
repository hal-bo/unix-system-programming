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
#include <signal.h>
#include <sys/errno.h>
#include <sys/time.h>

#ifndef MYDHCP_H
#define MYDHCP_H

#define SOCKET_PROTOCOL 0
#define SENDTO_FLAG 0
#define RECVFROM_FLAG 0

#define IP_LIST_SIZE_MAX 10
#define TIMEOUT_TIME 10
#define TIMER_INTERVAL 1

#define FILE_NAME_SIZE 256
#define TTL_TIME_STRSIZE 16

#define SERVER_STATE_INIT 1
#define SERVER_STATE_WAIT_REQUEST 2
#define SERVER_STATE_IN_USE 3
#define SERVER_STATE_REQUEST_TIMEOUT 4
#define SERVER_STATE_TERMINATE 5

#define CLIENT_STATE_INIT 1
#define CLIENT_STATE_WAIT_OFFER 2
#define CLIENT_STATE_WAIT_ACK 3
#define CLIENT_STATE_IN_USE 4
#define CLIENT_STATE_EXIT 5
#define CLIENT_STATE_OFFER_TIMEOUT 6
#define CLIENT_STATE_ACK_TIMEOUT 7
#define CLIENT_STATE_WAIT_ACK_EXTEND 8
#define CLIENT_STATE_ACK_EXTEND_TIMEOUT 9

#define SERVER_EVENT_RECEIVE_DISCOVER 1
#define SERVER_EVENT_RECEIVE_REQUEST_ALLOC 2
#define SERVER_EVENT_RECEIVE_REQUEST_EXTEND 3
#define SERVER_EVENT_REQUEST_ERROR 4
#define SERVER_EVENT_RECEIVE_TIMEOUT 5
#define SERVER_EVENT_TTL_TIMEOUT 6
#define SERVER_EVENT_RECEIVE_RELEASE 7

#define CLIENT_EVENT_INIT 1
#define CLIENT_EVENT_RECEIVE_OFFER_OK 2
#define CLIENT_EVENT_RECEIVE_OFFER_NG 3
#define CLIENT_EVENT_RECEIVE_ACK_OK 4
#define CLIENT_EVENT_RECEIVE_ACK_NG 5
#define CLIENT_EVENT_RECEIVE_SIGHUP 6
#define CLIENT_EVENT_TTL_HALF_PASSED 7
#define CLIENT_EVENT_RECEIVE_TIMEOUT 8


#define EVENT_NG 999
#define PACKET_ERROR 998
#define PASS_EVENT 997

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

struct ip_info {
    struct ip_info *fp;
    struct ip_info *bp;
    struct in_addr address;
    struct in_addr netmask;
    uint16_t ttl;
    int in_use;
};

struct client {
    struct client *fp;
    struct client *bp;
    int status;
    int ttlcounter;
    int toutcounter;

    struct sockaddr_in skt;
    struct ip_info assign_address;
    uint16_t ttl;
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
    struct mydhcp_message message;
};

struct proctable_server {
    int status;
    int event;
    void (*func)(struct event_table et);
};
struct proctable_client {
    int status;
    int event;
    void (*func)(struct event_table et);
};

void add_ip_info(uint16_t ttl, struct in_addr address, struct in_addr netmask);
void print_ip_info();
void read_config(char filename[FILE_NAME_SIZE]);
struct ip_info *offer_ip();
void release_ip(struct ip_info ip);
void mydhcp_server_offer(struct event_table et);
void mydhcp_server_ack(struct event_table et);
void mydhcp_server_release(struct event_table et);
void mydhcp_server_terminate(struct event_table et);
void set_server_status(struct client *c, int new_status);
char *server_state_to_str(int state);
void print_server_status(int old_state, int new_state, struct client *c);
void server_on_timer();
struct event_table wait_server_event();
void print_client();
struct client *search_client(struct sockaddr_in *skt);
struct client *add_client(struct sockaddr_in *skt);
void delete_client(struct client *c);
int get_server_event(struct client *c, struct mydhcp_message message);

void mydhcp_client_send_discover(struct event_table et);
void mydhcp_client_send_request(struct event_table et);
void mydhcp_client_set_ip(struct event_table et);
void mydhcp_client_send_release(struct event_table et);
void mydhcp_client_exit(struct event_table et);
char *client_state_to_str(int state);
void client_on_timer();
void print_client_status(int old_state, int new_state);
void set_client_status(int new_status);
struct event_table wait_client_event();

struct mydhcp_message create_message(
    uint8_t type,
    uint8_t code,
    uint16_t ttl,
    in_addr_t address,
    in_addr_t netmask
);
int get_event(struct mydhcp_message message);
void print_message(char *title, struct mydhcp_message message, struct sockaddr_in skt);
void print_event_table(struct event_table et);
void print_ip(char *title, struct sockaddr_in skt);
#endif