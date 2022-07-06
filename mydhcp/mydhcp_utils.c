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

void print_message(struct mydhcp_message message)
{
    char *type, *code = "";
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
    printf("-- print message --\n");
    printf("type -> %s\n", type);
    printf("code -> %s\n", code);
    printf("ttl -> %d", message.ttl);
    //printf("address -> %s")
    //printf("netmask");
    printf("\n");
}