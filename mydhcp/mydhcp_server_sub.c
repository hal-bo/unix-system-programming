#include "mydhcp.h"

void add_ip_info(uint16_t ttl, struct in_addr address, struct in_addr netmask)
{
    extern struct ip_info *ip_list_head;
    struct ip_info *item = (struct ip_info *)malloc(sizeof(struct ip_info));
    item->address = address;
    item->netmask = netmask;
    item->ttl = ttl;
    item->in_use = 0;

    item->fp = ip_list_head->fp;
    item->bp = ip_list_head;
    ip_list_head->fp->bp = item;
    ip_list_head->fp = item;
}

void print_ip_info()
{
    extern struct ip_info *ip_list_head;
    struct ip_info *p;
    printf("\n-- ip info --\n");
    for(p = ip_list_head->fp; p != ip_list_head; p = p->fp) {
        printf("\n  ip -> %s\n", inet_ntoa(p->address));
        printf("  netmask -> %s\n", inet_ntoa(p->netmask));
        if (p->in_use == 1) {
            printf("  in use\n");
        } else {
            printf("  not in use\n");
        }
    }
    printf("-- end --\n");
}
void read_config(char filename[FILE_NAME_SIZE])
{
    extern int default_ttl_time;
    struct in_addr address;
    struct in_addr netmask;
    FILE *fp;
    char default_ttl_time_str[TTL_TIME_STRSIZE];
    char addr_str[INET_ADDRSTRLEN];
    int i, j;
    char c;

    if ((fp = fopen(filename, "r")) == NULL) {
        perror("fopen");
        exit(1);
    }
    i = 0;
    while((c = fgetc(fp)) != '\n') {
        default_ttl_time_str[i++] = c;
    }
    default_ttl_time_str[i] = '\0';
    default_ttl_time = atoi(default_ttl_time_str);

    i = 0;
    j = 0;

    while(1) {
        c = fgetc(fp);
        if (c == '\n' || c == EOF) {
            addr_str[j] = '\0';
            netmask.s_addr = inet_addr(addr_str);
            add_ip_info(default_ttl_time,address, netmask);
            j = 0;
            if (c == EOF) {
                break;
            }
        } else if (c == ' ' || c == '\t') {
            addr_str[j] = '\0';
            address.s_addr = inet_addr(addr_str);
            j = 0;
        } else if (j == INET_ADDRSTRLEN - 1) {
            i++;
            addr_str[j] = '\0';
            fprintf(stderr, "config error: %s\n", addr_str);
            exit(1);
        } else {
            addr_str[j++] = c;
        }
    }

    fclose(fp);

    printf("\n-- read config --\n");
    printf("  default_ttl_time -> %d\n", default_ttl_time);
    print_ip_info();
    printf("\n-- end --\n");
}

struct ip_info *offer_ip()
{
    extern struct ip_info *ip_list_head;
    struct ip_info *p;
    for(p = ip_list_head->fp; p != ip_list_head; p = p->fp) {
        if (p->in_use == 0) {
            printf("\n-- offer ip --\n");
            printf("  ip -> %s\n", inet_ntoa(p->address));
            printf("  netmask -> %s\n", inet_ntoa(p->netmask));
            printf("-- end --\n");
            p->in_use = 1;
            p->bp->fp = p->fp;
            p->fp->bp = p->bp;
            p->bp = ip_list_head->bp;
            p->fp = ip_list_head;
            ip_list_head->bp->fp = p;
            ip_list_head->bp = p;
            print_ip_info();
            return p;
        }
    }
    print_ip_info();
    return NULL; 
}

void release_ip(struct ip_info ip)
{
    extern struct ip_info *ip_list_head;
    struct ip_info *p;
    for(p = ip_list_head->fp; p != ip_list_head; p = p->fp) {
        if (p->address.s_addr == ip.address.s_addr
            && p->netmask.s_addr == ip.netmask.s_addr) {
                p->in_use = 0;
            }
    }
    print_ip_info();
}

void mydhcp_server_offer(struct event_table et)
{
    extern int sock_fd;
    extern int server_timecount;
    struct mydhcp_message message;
    int count;
    struct ip_info *ip = offer_ip();

    if (ip) {
        et.client->assign_address = *ip;
        message = create_message(TYPE_OFFER, OFFER_CODE_OK, ip->ttl, ip->address.s_addr, ip->netmask.s_addr);
        et.client->toutcounter = TIMEOUT_TIME + server_timecount;
    } else {
        message = create_message(TYPE_OFFER, OFFER_CODE_NG, IGNORE, IGNORE, IGNORE);
    }
    if ((count = sendto(sock_fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)&et.client->skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    print_message("send message", message, et.client->skt);
    if (ip == NULL) {
        delete_client(et.client);
        return;
    }
    switch (et.client->status) {
        case SERVER_STATE_INIT:
            set_server_status(et.client, SERVER_STATE_WAIT_REQUEST);
            break;
        case SERVER_STATE_WAIT_REQUEST:
            set_server_status(et.client, SERVER_STATE_REQUEST_TIMEOUT);
            break;
        default:
            fprintf(stderr, "invalid state\n");
            exit(1);
    }
}

void mydhcp_server_ack(struct event_table et)
{
    extern int sock_fd;
    extern int server_timecount;
    struct mydhcp_message message;
    int count;

    et.client->ttl = et.message.ttl;
    et.client->ttlcounter = et.client->ttl + server_timecount;
    message = create_message(TYPE_ACK, ACK_CODE_OK, et.message.ttl, et.message.address, et.message.netmask);
    if ((count = sendto(sock_fd, &message, sizeof(message), SENDTO_FLAG, (struct sockaddr *)&et.client->skt, sizeof(struct sockaddr_in))) < 0) {
        perror("sendto");
        exit(1);
    }
    print_message("send message", message, et.client->skt);
    switch (et.client->status) {
        case SERVER_STATE_WAIT_REQUEST:
            set_server_status(et.client, SERVER_STATE_IN_USE);
            break;
        case SERVER_STATE_IN_USE:
            set_server_status(et.client, SERVER_STATE_IN_USE);
            break;
        case SERVER_EVENT_RECEIVE_TIMEOUT:
            set_server_status(et.client, SERVER_STATE_IN_USE);
            break;
        default:
            fprintf(stderr, "invalid state");
            exit(1);
    }
}

void mydhcp_server_release(struct event_table et)
{
    release_ip(et.client->assign_address);
    mydhcp_server_terminate(et);
}

void mydhcp_server_terminate(struct event_table et)
{
    print_server_status(et.client->status, SERVER_STATE_TERMINATE, et.client);
    delete_client(et.client);
}

void set_server_status(struct client *c, int new_status)
{
    print_server_status(c->status, new_status, c);
    c->status = new_status;
}

char *server_state_to_str(int state)
{
    char *str = "UNKNOWN STATE";
    switch (state) {
        case SERVER_STATE_INIT:
            str = "INIT";
            break;
        case SERVER_STATE_WAIT_REQUEST:
            str = "WAIT_REQUEST";
            break;
        case SERVER_STATE_IN_USE:
            str = "IN_USE";
            break;
        case SERVER_STATE_REQUEST_TIMEOUT:
            str = "REQUEST_TIMEOUT";
            break;
        case SERVER_STATE_TERMINATE:
            str = "TERMINATE";
            break;
    }
    return str;
}

void print_server_status(int old_state, int new_state, struct client *c)
{
    char *old_state_str = server_state_to_str(old_state);
    char *new_state_str = server_state_to_str(new_state);
    printf("\n[ ");
    printf("%s -> %s", old_state_str, new_state_str);
    printf(" ]");
    char *ip = inet_ntoa(c->skt.sin_addr);
    printf(" client(%s:%d)\n\n", ip, ntohs(c->skt.sin_port));
}

void server_on_timer() {
    extern struct client *client_list_head;
    extern int server_timecount;
    struct client *cp;
    struct event_table et;
    int i;

    for (i=0;i<server_timecount;i++){
        write(1, ".", 2);
    }
    cp = client_list_head->fp;
    while (cp != client_list_head) {
        switch (cp->status) {
            case SERVER_STATE_WAIT_REQUEST:
                cp->toutcounter -= server_timecount;
                if (cp->toutcounter < 0) {
                    et.client = cp;
                    printf("\n!! REQUEST TIMEOUT !!\n");
                    mydhcp_server_offer(et);
                }
                cp = cp->fp;
                break;
            case SERVER_STATE_REQUEST_TIMEOUT:
                cp->toutcounter -= server_timecount;
                if (cp->toutcounter < 0) {
                    et.client = cp;
                    printf("\n!! RETRY REQUEST TIMEOUT !!\n");
                    mydhcp_server_offer(et);
                }
                cp = cp->fp;
                break;
            case SERVER_STATE_IN_USE:
                cp->ttlcounter -= server_timecount;
                if (cp->ttlcounter < 0) {
                    et.client = cp;
                    cp = cp->fp;
                    printf("\n!! TTL TIMEOUT !!\n");
                    mydhcp_server_release(et);
                } else {
                    cp = cp->fp;
                }
                break;
        }
    }
    server_timecount = 0;
}

struct event_table wait_server_event()
{
    extern int sock_fd;
    extern struct sockaddr_in myskt;
    struct event_table et;
    fd_set rdfds;
    struct timeval timer;
    struct sockaddr_in skt;
    int ret_val;

    memset(&skt, 0, sizeof(skt));
    skt.sin_family = AF_INET;
    skt.sin_port = htons(CLIENT_PORT);
    skt.sin_addr.s_addr = htonl(INADDR_ANY);

    timer.tv_sec = 1;
    timer.tv_usec = 0;
    FD_ZERO(&rdfds);
    FD_SET(STDOUT_FILENO, &rdfds);
    FD_SET(sock_fd, &rdfds);

    et.event = PASS_EVENT;
    ret_val = select(sock_fd+1, &rdfds, NULL, NULL, NULL);
    if (ret_val < 0) {
        if (errno == EINTR) {
            server_on_timer();
            et.event = PASS_EVENT;
            return et;
        }
        perror("select");
        exit(1);
    } else if (ret_val == 0) {
        et.event = EVENT_NG;
        return et;
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
            print_message("receive message", message, skt);
            et.client = search_client(&skt);
            if (et.client == NULL) {
                if (message.type != TYPE_DISCOVER) {
                    et.event = EVENT_NG;
                    return et;
                }
                et.client = add_client(&skt);
            }
            et.message = message;
            et.event = get_server_event(et.client, et.message);
            return et;
        }
    }

    return et;
}

struct client *search_client(struct sockaddr_in *skt)
{
    extern struct client *client_list_head;
    struct client *head;
    struct client *cp;

    head = client_list_head;
    for (cp = head->fp;cp != head;cp = cp->fp) {
        if (inet_ntoa(skt->sin_addr) == inet_ntoa(cp->skt.sin_addr)
            && skt->sin_port ==  cp->skt.sin_port) {
            return cp;
        }
    }
    return NULL;
}

void print_client()
{
    extern struct client *client_list_head;
    struct client *cp;

    printf("\n-- client list --\n");
    for (cp = client_list_head->fp;cp != client_list_head;cp = cp->fp) {
        char *ip = inet_ntoa(cp->skt.sin_addr);
        printf("  client(%s:%d)\n\n", ip, ntohs(cp->skt.sin_port));
    }
    printf("-- end --\n");
}

struct client *add_client(struct sockaddr_in *skt)
{
    extern struct client *client_list_head;
    struct client *new_client = (struct client *)malloc(sizeof(struct client));
    
    new_client->status = SERVER_STATE_INIT;
    new_client->skt = *skt;

    client_list_head->fp->bp = new_client;
    new_client->fp = client_list_head->fp;
    client_list_head->fp = new_client;
    new_client->bp = client_list_head;

    printf("\n-- add client --\n");
    print_client();
    return new_client;
}

void delete_client(struct client *c)
{
    extern struct client *client_list_head;
    struct client *cp;

    for (cp = client_list_head->fp;cp != client_list_head;cp = cp->fp) {
        if (inet_ntoa(c->skt.sin_addr) == inet_ntoa(cp->skt.sin_addr)) {
            cp->bp->fp = cp->fp;
            cp->fp->bp = cp->bp;
            free(c);
            c = NULL;
            printf("\n-- delete client --\n");
            print_client();
            return;
        }
    }
    fprintf(stderr, "client not found\n");
    exit(1);
}

int get_server_event(struct client *c, struct mydhcp_message message)
{
    switch (message.type) {
        case TYPE_DISCOVER:
            return SERVER_EVENT_RECEIVE_DISCOVER;
        case TYPE_REQUEST:
            if (c->assign_address.address.s_addr != message.address
                    || c->assign_address.netmask.s_addr != message.netmask) {
                return SERVER_EVENT_REQUEST_ERROR;
            }
            if (c->assign_address.ttl < message.ttl) {
                return SERVER_EVENT_REQUEST_ERROR;
            } else {
                c->ttl = message.ttl;
            }
            switch (message.code) {
                case REQUEST_CODE_ALLOCK:
                    return SERVER_EVENT_RECEIVE_REQUEST_ALLOC;
                case REQUEST_CODE_EXTEND:
                    return SERVER_EVENT_RECEIVE_REQUEST_EXTEND;
                default:
                    return EVENT_NG;
            }
        case TYPE_RELEASE:
            return SERVER_EVENT_RECEIVE_RELEASE;
        default:
            return EVENT_NG;
    }
    return EVENT_NG;
}