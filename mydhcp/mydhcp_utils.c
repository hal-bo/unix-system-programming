#include "mydhcp.h"

struct mydhcp_message create_message(
    uint8_t type,
    uint8_t code,
    uint16_t ttl,
    in_addr_t address,
    in_addr_t netmask
)
{
    struct mydhcp_message message;

    message.type = type;
    message.code = code;
    message.ttl = ttl;
    message.address = address;
    message.netmask = netmask;

    return message;
}

int get_event(struct mydhcp_message message)
{
    switch (message.type) {
        case TYPE_DISCOVER:
            return SERVER_EVENT_RECEIVE_DISCOVER;
        case TYPE_OFFER:
            switch (message.code) {
                case OFFER_CODE_OK:
                    return CLIENT_EVENT_RECEIVE_OFFER_OK;
                case OFFER_CODE_NG:
                    return CLIENT_EVENT_RECEIVE_OFFER_NG;
                default:
                    return PACKET_ERROR;
            }
            break;
        case TYPE_REQUEST:
            switch (message.code) {
                case REQUEST_CODE_ALLOCK:
                    return SERVER_EVENT_RECEIVE_REQUEST_ALLOC;
                case REQUEST_CODE_EXTEND:
                    return SERVER_EVENT_RECEIVE_REQUEST_EXTEND;
                default:
                    return PACKET_ERROR;
            }
        case TYPE_ACK:
            switch (message.code) {
                case ACK_CODE_OK:
                    return CLIENT_EVENT_RECEIVE_ACK_OK;
                case ACK_CODE_NG:
                    return CLIENT_EVENT_RECEIVE_ACK_NG;
                default:
                    return PACKET_ERROR;
            }
        case TYPE_RELEASE:
            return SERVER_EVENT_RECEIVE_RELEASE;
        default:
            return PACKET_ERROR;
    }
    return PACKET_ERROR;
}

void print_message(char *title, struct mydhcp_message message, struct sockaddr_in skt)
{
    char *ip = inet_ntoa(skt.sin_addr);
    printf("\n-- %s (%s:%d) --\n", title, ip, ntohs(skt.sin_port));
    char *type, *code = "NO CODE";
    struct in_addr address, netmask;
    char address_str[INET_ADDRSTRLEN], netmask_str[INET_ADDRSTRLEN];
    switch (message.type) {
        case TYPE_DISCOVER:
            type = "DISCOVER";
            break;
        case TYPE_OFFER:
            type = "OFFER";
            switch (message.code) {
                case OFFER_CODE_OK:
                    code = "OK";
                    break;
                case OFFER_CODE_NG:
                    code = "NG";
                    break;
                default:
                    code = "UNKNOWN CODE";
                    break;
            }
            break;
        case TYPE_REQUEST:
            type = "REQUEST";
            switch (message.code) {
                case REQUEST_CODE_ALLOCK:
                    code = "ALLOCK";
                    break;
                case REQUEST_CODE_EXTEND:
                    code = "EXTEND";
                    break;
                default:
                    code = "UNKNOWN CODE";
                    break;
            }
            break;
        case TYPE_ACK:
            type = "ACK";
            switch (message.code) {
                case ACK_CODE_OK:
                    code = "OK";
                    break;
                case ACK_CODE_NG:
                    code = "NG";
                    break;
                default:
                    code = "UNKNOWN CODE";
                    break;
            }
            break;
        case TYPE_RELEASE:
            type = "RELEASE";
            break;
        default:
            type = "UNKNOWN TYPE";
            break;
    }
    address.s_addr = message.address;
    inet_ntop(AF_INET, &address, address_str, sizeof(address_str));
    netmask.s_addr = message.netmask;
    inet_ntop(AF_INET, &netmask, netmask_str, sizeof(netmask_str));

    printf("  type -> %s\n", type);
    printf("  code -> %s\n", code);
    printf("  ttl -> %d\n", message.ttl);
    printf("  address -> %s\n", address_str);
    printf("  netmask -> %s\n", netmask_str);
    printf("-- end --\n");
}

void print_event_table(struct event_table et)
{
    printf("\n-- print event_table --\n");
    if (et.event == EVENT_NG) {
        fprintf(stderr, "EVENT_NG");
        return;
    }
    printf("event -> ");
    switch (et.event) {
        case SERVER_EVENT_RECEIVE_DISCOVER:
            printf("SERVER_EVENT_RECEIVE_DISCOVER\n");
            break;
        case SERVER_EVENT_RECEIVE_REQUEST_ALLOC:
            printf("SERVER_EVENT_RECEIVE_REQUEST_ALLOC\n");
            break;
        case SERVER_EVENT_RECEIVE_REQUEST_EXTEND:
            printf("SERVER_EVENT_RECEIVE_REQUEST_EXTEND\n");
            break;
        case SERVER_EVENT_REQUEST_ERROR:
            printf("SERVER_EVENT_REQUEST_ERROR\n");
            break;
        case SERVER_EVENT_RECEIVE_TIMEOUT:
            printf("SERVER_EVENT_RECEIVE_TIMEOUT\n");
            break;
        case SERVER_EVENT_TTL_TIMEOUT:
            printf("SERVER_EVENT_TTL_TIMEOUT\n");
            break;
        case SERVER_EVENT_RECEIVE_RELEASE:
            printf("SERVER_EVENT_RECEIVE_RELEASE\n");
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
        case SERVER_STATE_REQUEST_TIMEOUT:
            printf("SERVER_STATE_REQUEST_TIMEOUT\n");
            break;
        case SERVER_STATE_TERMINATE:
            printf("SERVER_STATE_TERMINATE\n");
            exit(0);
            break;
        default:
            printf("UNKNOWN STATE\n");
            break;
    }
    printf("-- end --\n");
}

void print_ip(char *title, struct sockaddr_in skt)
{
    char *ip = inet_ntoa(skt.sin_addr);
    printf("%s -> %s\n", title, ip);
}