#include "server.h"

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

const Options OPTIONS_DEFAULT = {
    .quality_variable = false,
    .quality = 0,
    .chunks_forward = 3,
    .chunks_backward = 2
};

int init_server(uint16_t port) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in name = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    socklen_t name_size = sizeof(name);

    if(bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(sock, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int client = accept(sock, (struct sockaddr *) &name, &name_size);
    if(client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    char *response = "Hello, world!";
    ssize_t request = read(client, buffer, sizeof(buffer) - 1);
    send(client, response, strlen(response), 0);

    return sock;
}

void close_server(int sock) {
    // shutdown(sock, 2);
    close(sock);
}
