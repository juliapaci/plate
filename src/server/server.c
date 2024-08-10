#include "server.h"
#include <protocol.h>

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
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
        .root = opendir(".")
    };
}

void init_server(Server *state) {
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

Packet process_request(Packet *request, Server *server) {
    Packet response = SERVER_PACKET;

    if(request->header.whence == SERVER)
        response.header.res_kind = FAILURE;

    switch(request->header.req_kind) {
        case HANDSHAKE:
            // Should be done in client handler
            break;
        case LIST:
            response.body.list = fetch_list(server->root);
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
    ResponseKind response = HANDSHAKE_SUCCESS;
    recv(state.client, &handshake, sizeof(Handshake), 0);
    if(!validate_handshake(&handshake))
        response = HANDSHAKE_FAILURE;
    send(state.client, &response, sizeof(response), 0);

    while(true) {
        Packet request = CLIENT_PACKET;
        recv(state.client, &request.header.req_kind, sizeof(RequestKind), 0);
        if(request.header.req_kind == EXIT)
            break;

        Packet response = process_request(&request, state.server_state);
        respond(state.client, request.header.req_kind, &response);
    }

    close(state.client);
    return NULL;
}

// TODO: need a better way to keep track of what should be free()d
char **fetch_list(DIR *dirp) {
    if(!dirp) {
        fprintf(stderr, "failed to fetch list");
    }

    char **response = malloc(1 * sizeof(char *));
    struct dirent *dir;
    for(size_t i = 0; (dir = readdir(dirp)) != NULL; i++) {
        response = realloc(response, (i+1) * sizeof(char *));
        // TODO: symlinks?
        response[i] = dir->d_name;
    }

    return response;
}

void clean_state(Server *server) {
    closedir(server->root);
}
