#include "server.h"
#include <protocol.h>

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

const Options OPTIONS_DEFAULT = {
    .quality_variable = false,
    .quality = 0,
    .chunks_forward = 3,
    .chunks_backward = 2,
    // chunk_size = // TODO: find unit
};

// Options parse_options(int argc, char **argv) {
    // TODO: do with getopt()
// }

Server init_state(uint16_t port) {
    return (Server){
        .port = port,
        .root = "."
    };
}

void init_server(Server *state) {
    const int server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const int opt = 1;
    // TODO: socket timout for not TCP ACK
    // TODO: maybe SO_KEEPALIVE
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_sockaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(state->port),
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
        pthread_create(&thread, NULL, _handle_client, (void *)&(struct SharedState){
            client,
            state
        });
        pthread_detach(thread);
    }

    close(server);
    exit(EXIT_SUCCESS);
}

Packet process_request(RequestKind req, Server *server) {
    Packet response = {0};

    switch(req) {
        case HANDSHAKE:
            // Should be done in client handler
            break;
        case LIST:
            response = command_fetch_list(server->root);
            break;
    }

    return response;
}

void *_handle_client(void *arg) {
    const struct SharedState state = *(struct SharedState *)arg;

    if(state.client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    Handshake handshake;
    recv(state.client, &handshake, sizeof(Handshake), 0);
    bool valid = validate_handshake(&handshake);
    send(state.client, &valid, sizeof(valid), 0);
    if(!valid) {
        close(state.client);
        return NULL;
    }

    while(true) {
        RequestKind request;
        recv(state.client, &request, sizeof(request), 0);
        if(request == EXIT)
            break;

        Packet response = process_request(request, state.server_state);
        respond(state.client, request, &response, 0);
        free(response.raw);
    }

    close(state.client);
    return NULL;
}

void clean_state(Server *server) { }
