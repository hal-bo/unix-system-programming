#include "mydhcp.h"

void mydhcp_client_send_discover(struct event_table et)
{
    extern int sock_fd;
    extern struct client info;
    extern int client_timecount;
    extern struct sockaddr_in skt;
    struct mydhcp_message message;
    int count;

    message = create_message(TYPE_DISCOVER, IGNORE, IGNORE, IGNORE, IGNORE);
    if ((count = sendto(sock_fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)&skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    info.toutcounter = TIMEOUT_TIME + client_timecount;
    print_message("send message", message, skt);
    switch (info.status) {
        case CLIENT_STATE_INIT:
            set_client_status(CLIENT_STATE_WAIT_OFFER);
            break;
        case CLIENT_STATE_WAIT_OFFER:
            set_client_status(CLIENT_STATE_OFFER_TIMEOUT);
            break;
        default:
            fprintf(stderr, "invalid state\n");
            exit(1);
    }
}

void mydhcp_client_send_request(struct event_table et)
{
    extern struct client info;
    extern int client_timecount;
    extern int sock_fd;
    extern struct sockaddr_in skt;
    struct mydhcp_message message;
    int count;

    if (et.client) { // RESEND ではない場合
        printf("first");
        info.ttl = et.message.ttl;
        info.assign_address.address.s_addr = et.message.address;
        info.assign_address.netmask.s_addr = et.message.netmask;
    } else {
        printf("second %d", info.ttl);
    }

    if (info.status == CLIENT_STATE_WAIT_OFFER || info.status == CLIENT_STATE_WAIT_ACK) {
        message = create_message(TYPE_REQUEST, REQUEST_CODE_ALLOCK, info.ttl, info.assign_address.address.s_addr, info.assign_address.netmask.s_addr);
    } else {
        message = create_message(TYPE_REQUEST, REQUEST_CODE_EXTEND, info.ttl, info.assign_address.address.s_addr, info.assign_address.netmask.s_addr);
    }
    if ((count = sendto(sock_fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)&skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    info.toutcounter = TIMEOUT_TIME + client_timecount;
    print_message("send message", message, skt);
    switch (info.status) {
        case CLIENT_STATE_WAIT_OFFER:
            set_client_status(CLIENT_STATE_WAIT_ACK);
            break;
        case CLIENT_STATE_WAIT_ACK:
            set_client_status(CLIENT_STATE_ACK_TIMEOUT);
            break;
        case CLIENT_STATE_IN_USE:
            set_client_status(CLIENT_STATE_WAIT_ACK_EXTEND);
            break;
        case CLIENT_STATE_OFFER_TIMEOUT:
            set_client_status(CLIENT_STATE_WAIT_ACK);
            break;
        case CLIENT_STATE_WAIT_ACK_EXTEND:
            set_client_status(CLIENT_STATE_ACK_EXTEND_TIMEOUT);
            break;
        default:
            fprintf(stderr, "invalid state\n");
            exit(1);
    }
}

void mydhcp_client_set_ip(struct event_table et)
{
    extern int hup_flag;
    extern struct client info;
    extern int client_timecount;
    struct in_addr address, netmask;
    struct itimerval val;
    char address_str[INET_ADDRSTRLEN], netmask_str[INET_ADDRSTRLEN];

    address.s_addr = et.message.address;
    inet_ntop(AF_INET, &address, address_str, sizeof(address_str));
    netmask.s_addr = et.message.netmask;
    inet_ntop(AF_INET, &netmask, netmask_str, sizeof(netmask_str));

    printf("\n--ip address assigned --\n");
    printf("ttl -> %d\n", et.message.ttl);
    printf("address -> %s\n", address_str);
    printf("netmask -> %s\n", netmask_str);
    printf("-- end --\n");

    hup_flag = 0;

    info.ttl = et.message.ttl;
    info.ttlcounter = info.ttl + client_timecount;
    info.assign_address.address.s_addr = et.message.address;
    info.assign_address.netmask.s_addr = et.message.netmask;
    
    set_client_status(CLIENT_STATE_IN_USE);
}

void mydhcp_client_send_release(struct event_table et)
{
    extern int sock_fd;
    extern struct sockaddr_in skt;
    struct mydhcp_message message;
    int count;

    message = create_message(TYPE_RELEASE, IGNORE, IGNORE, IGNORE, IGNORE);
    if ((count = sendto(sock_fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)&skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    print_message("send message", message, skt);
    mydhcp_client_exit(et);
}

void mydhcp_client_exit(struct event_table et)
{
    extern int sock_fd;
    extern struct client info;
    print_client_status(info.status, CLIENT_STATE_EXIT);
    close(sock_fd);
    exit(0);
}

char *client_state_to_str(int state)
{
    char *str = "UNKNOWN STATE";
    switch (state) {
        case CLIENT_STATE_INIT:
            str = "INIT";
            break;
        case CLIENT_STATE_WAIT_OFFER:
            str = "WAIT_OFFER";
            break;
        case CLIENT_STATE_WAIT_ACK:
            str = "WAIT_ACK";
            break;
        case CLIENT_STATE_IN_USE:
            str = "IN_USE";
            break;
        case CLIENT_STATE_OFFER_TIMEOUT:
            str = "OFFER_TIMEOUT";
            break;
        case CLIENT_STATE_ACK_TIMEOUT:
            str = "ACK_TIMEOUT";
            break;
        case CLIENT_STATE_WAIT_ACK_EXTEND:
            str = "WAIT_ACK_EXTEND";
            break;
        case CLIENT_STATE_ACK_EXTEND_TIMEOUT:
            str = "ACK_EXTEND_TIMEOUT";
            break;
        case CLIENT_STATE_EXIT:
            str = "EXIT";
            break;
    }
    return str;
}

void print_client_status(int old_state, int new_state)
{
    char *old_state_str = client_state_to_str(old_state);
    char *new_state_str = client_state_to_str(new_state);
    printf("\n[ ");
    printf("%s -> %s", old_state_str, new_state_str);
    printf(" ]\n");
}

void set_client_status(int new_status)
{
    extern struct client info;
    print_client_status(info.status, new_status);
    info.status = new_status;
}

void client_on_timer()
{
    extern struct client info;
    extern int client_timecount;
    struct event_table et;
    int i;

    et.client = NULL;
    for (i=0;i<client_timecount;i++){
        write(1, ".", 2);
    }
    switch (info.status) {
        case CLIENT_STATE_WAIT_OFFER:
            info.toutcounter -= client_timecount;
            if (info.toutcounter < 0) {
                printf("\n!! OFFER TIMEOUT !!\n");
                mydhcp_client_send_discover(et);
            }
            break;
        case CLIENT_STATE_WAIT_ACK:
            info.toutcounter -= client_timecount;
            if (info.toutcounter < 0) {
                printf("\n!! ALLOC ACK TIMEOUT !!\n");
                mydhcp_client_send_request(et);
            }
            break;
        case CLIENT_STATE_OFFER_TIMEOUT:
            info.toutcounter -= client_timecount;
            if (info.toutcounter < 0) {
                printf("\n!! RETRY OFFER TIMEOUT !!\n");
                mydhcp_client_exit(et);
            }
            break;
        case CLIENT_STATE_WAIT_ACK_EXTEND:
            info.toutcounter -= client_timecount;
            if (info.toutcounter < 0) {
                printf("\n!! EXTEND ACK TIMEOUT !!\n");
                mydhcp_client_send_request(et);
            }
            break;
        case CLIENT_STATE_ACK_EXTEND_TIMEOUT:
            info.toutcounter -= client_timecount;
            if (info.toutcounter < 0) {
                printf("\n!! RETRY EXTEND ACK TIMEOUT !!\n");
                mydhcp_client_exit(et);
            }
            break;
        case CLIENT_STATE_IN_USE:
            info.ttlcounter -= client_timecount;
            if (info.ttlcounter <= info.ttl/2) {
                printf("\n!! TTL HALF PASSED !!\n");
                mydhcp_client_send_request(et);
            }
    }
    client_timecount = 0;
}

struct event_table wait_client_event()
{
    extern int hup_flag;
    extern int client_timecount;
    extern int sock_fd;
    extern struct sockaddr_in skt;
    extern struct client info;
    struct event_table et;
    fd_set rdfds;
    struct timeval timeout;
    int ret_val;

    FD_ZERO(&rdfds);
    FD_SET(STDOUT_FILENO, &rdfds);
    FD_SET(sock_fd, &rdfds);

    ret_val = select(sock_fd+1, &rdfds, NULL, NULL, NULL);

    if (ret_val < 0) {
        if (errno == EINTR) {
            if (hup_flag == 1) {
                mydhcp_client_send_release(et);
            } else if (client_timecount) {
                client_on_timer();
            }
            et.event = PASS_EVENT;
            return et;
        }
        perror("select");
        exit(1);
    } else if (ret_val == 0) {
    } else {
        if (FD_ISSET(STDOUT_FILENO, &rdfds)) {
            
        }
        if (FD_ISSET(sock_fd, &rdfds)) {
            int count;
            struct mydhcp_message message;
            socklen_t sktlen = sizeof(struct sockaddr);
            if ((count = recvfrom(sock_fd, &message, sizeof(message), RECVFROM_FLAG, (struct sockaddr *)&skt, &sktlen)) < 0) {
                perror("recvfrom");
                exit(1);
            }
            et.message = message;
            print_message("receive message", message, skt);
            if ((et.event = get_event(message)) == EVENT_NG) {
                fprintf(stderr, "UNKONWN EVENT ERROR\n");
                exit(1);
            }
            return et;
        }
    }
    
    return et;
}
