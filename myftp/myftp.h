#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/select.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>

#define DATASIZE 1024

#define SERVER_PORT 50021
#define SERVER_PORT_STR "50021"
#define CLIENT_PORT 50022

struct myftph {
    uint8_t type;
    uint8_t code;
    uint16_t length;
};
struct myftph_data {
    uint8_t type;
    uint8_t code;
    uint16_t length;
    char data[DATASIZE];
};

void set_addrinfo(char *host_str, struct addrinfo *server_addr);
