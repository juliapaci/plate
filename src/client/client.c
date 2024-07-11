#include "client.h"
#include <protocol.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void client_connect(uint16_t port) {
    const int client = socket(PF_INET, SOCK_STREAM, 0);
    if(client < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in client_sockaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if(inet_pton(AF_INET, "0.0.0.0", &client_sockaddr.sin_addr) < 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if(connect(client, (struct sockaddr *) &client_sockaddr, sizeof(client_sockaddr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    const Handshake handshake = HANDSHAKE_EXPECTED;
    send(client, &handshake, sizeof(Handshake), 0);
    char buffer[1024] = {0};
    recv(client, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    close(client);
}
