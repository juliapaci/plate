#include "server.h"
#include <protocol.h>

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

const Options OPTIONS_DEFAULT = {
    .quality_variable = false,
    .quality = 0,
    .chunks_forward = 3,
    .chunks_backward = 2
};

void init_server(uint16_t port) {
    const int server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const int opt = 1;
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_sockaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    struct sockaddr_in client_sockaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);

    if(bind(server, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(server, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true) {
        int client = accept(server, (struct sockaddr *) &client_sockaddr, &socklen);

        pthread_t thread;
        pthread_create(&thread, NULL, _handle_client, (void *)&client);
        // TODO: detach is what we want but it doesnt work for some reason?
        pthread_join(thread, NULL);

        close(server);
        exit(EXIT_SUCCESS);
    }
}

void *_handle_client(void *arg) {
    const int client = *(int *)arg;

    if(client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = malloc(sizeof(Handshake));
    const char *response = "hi from server";
    recv(client, buffer, sizeof(buffer), 0);
    validate_handshake(buffer);

    send(client, response, strlen(response), 0);

    // free(arg);
    close(client);
    return NULL;
}
