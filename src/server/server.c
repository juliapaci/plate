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

Server init_state(void) {
    char cwd[PATH_MAX] = ".";
    // if(getcwd(cwd, sizeof(cwd)) == NULL) {
    //     perror("getcwd");
    //     exit(EXIT_FAILURE);
    // }

    return (Server){
        .living = opendir(cwd)
    };
}

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
        pthread_detach(thread);
    }

    close(server);
    exit(EXIT_SUCCESS);
}

void *_handle_client(void *arg) {
    const int client = *(int *)arg;

    if(client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
unsigned char *buffer = malloc(sizeof(Handshake)); char *response = "s";
    // Packet response = {
    //     .kind = HANDSHAKE_SUCCESS
    // };
    recv(client, buffer, sizeof(buffer), 0);
    if(!validate_handshake(buffer)) {
        response = "failed to validate the appropriate handshake";
    }

    send(client, response, strlen(response), 0);
    fetch_list(opendir("."));

    close(client);
    return NULL;
}

char *fetch_list(DIR *dirp) {
    if(!dirp) {
        fprintf(stderr, "failed to fetch list");
    }

    char *response = malloc(0);

    struct dirent *dir;
    while((dir = readdir(dirp)) != NULL) {
        printf("%s\n", dir->d_name);
    }

    return response;
}

void clean_state(Server *server) {
    closedir(server->living);
}
