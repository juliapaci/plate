#include "server.h"

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

int init_server(Options *options) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in name = {
        .sin_family = AF_INET,
        .sin_port = htons(options->port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    if(bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sock;
}
