#include "myftp.h"

void set_addrinfo(char *host_str, struct addrinfo *server_addr)
{
    struct addrinfo hints;
    int err;
    char output[256];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(host_str, SERVER_PORT_STR, &hints, &server_addr)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    getnameinfo(server_addr->ai_addr, server_addr->ai_addrlen, output, sizeof(output), NULL, 0, NI_NUMERICHOST);
    printf("\n-- set addrinfo --\n");
    printf("    hostname -> %s\n", host_str);
    printf("    ip       -> %s\n", output);
    printf("-- end --\n");
}