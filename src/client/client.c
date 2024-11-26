#include "client.h"
#include <protocol.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>

void client_connect(uint16_t port) {
    const int server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
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

    if(connect(server, (struct sockaddr *) &client_sockaddr, sizeof(client_sockaddr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    const Handshake handshake = HANDSHAKE_EXPECTED;
    send(server, &handshake, sizeof(Handshake), 0);
    bool valid;
    recv(server, &valid, sizeof(valid), 0);
    if(valid != true) {
        fprintf(stderr, "handshake failed\n");
        close(server);
        exit(EXIT_FAILURE);
    }

    while(true) {
        RequestKind command = debug_control(); // sync
        size_t args;
        if(command == METADATA)
            scanf("%ld", &args);

        Packet response = request(server, (Request){
            .kind = command,
            .args = args
        });

        printf("%s\n", (char *)response.raw);
        free(response.raw);

        if(command == EXIT)
            break;
    }
}

RequestKind debug_control(void) {
    char command[MAX_INPUT];
    scanf("%s", command);

    return string_request_direct(command);
}
