#include "server.h"

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_LEN 1024

const Options OPTIONS_DEFAULT = {
    .quality_variable = false,
    .quality = 0,
    .chunks_forward = 3,
    .chunks_backward = 2
};

void init_server(uint16_t port) {
    int server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
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

    char buffer[BUFFER_LEN];
    const char *response = "Hello, world!";

    while(true) {
        int client = accept(server, (struct sockaddr *) &client_sockaddr, &socklen);

        __pid_t pid = fork();

        if(pid != 0) close(client);

        if(client < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        read(client, buffer, BUFFER_LEN);
        printf("%s\n", buffer);
        send(client, response, strlen(response), 0);

        close(server);
        close(client);
        exit(EXIT_SUCCESS);
    }
}
