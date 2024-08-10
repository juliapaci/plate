#include "protocol.h"

#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

const char *request_string_direct(RequestKind request) {
    char *string;
    switch(request) {
        case HANDSHAKE:
            string = "handshake";
            break;
        case METADATA:
            string = "metadata";
            break;
        case LOAD:
            string = "load";
            break;
        case LIST:
            string = "list";
            break;
        case EXIT:
            string = "exit";
            break;
        default:
            string = "unknown";
            break;
    }

    return string;
}

RequestKind string_request_direct(char *request) {
    if(strcmp(request, "list") == 0)
        return LIST;
    else if(strcmp(request, "exit") == 0)
        return EXIT;

    return EXIT;
}

const char *response_string(ResponseKind response) {
    char *string;
    switch(response) {
        case PRELUDE:
            string = "failed to confirm prelude";
            break;
        case FAILURE:
            string = "error: failed";
            break;
        case HANDSHAKE_FAILURE:
            string = "failed to verify the handshake";
            break;
        default:
            string = "success";
            break;
    }

    return string;
}

size_t body_size(Body body, RequestKind req) {
    size_t size = 0;

    switch(req) {
        case LIST:
            for(size_t i = 0; i < 1 /* sizeof(body.list)/sizeof(char *) */; i++)
                size += strlen(body.list[i]);
            break;
        case EXIT:
            size = 0;
    }

    return size;
}

void respond(int client_fd, RequestKind req, Packet *packet) {
    // send the prelude
    Packet confirmation = SERVER_PACKET;
    confirmation.body.size = body_size(packet->body, req);
    send(client_fd, &confirmation, sizeof(confirmation), 0);

    send(client_fd, &packet->header, sizeof(PacketHeader), 0);
    packet->body.raw = "testa";
    send(client_fd, packet->body.raw, confirmation.body.size, 0);
}

// TODO: actual error handling or propagate
// returns SERVER_PACKET on error
Packet request(int server_fd, RequestKind req) {
    // recieve confirmation
    // send(server_fd, packet, sizeof(Packet), 0);
    send(server_fd, &req, sizeof(RequestKind), 0);
    Packet confirmation;
    recv(server_fd, &confirmation, sizeof(Packet), 0);
    if(!validate_confirmation(&confirmation)) {
        PRINT_RESPONSE(confirmation.header.res_kind);
        return SERVER_PACKET;
    }

    Packet response = CLIENT_PACKET;
    recv(server_fd, &response.header, sizeof(PacketHeader), 0);
    recv(server_fd, &response.body.raw, confirmation.body.size, 0);

    return response;
}

extern bool validate_handshake(Handshake *handshake);
extern bool validate_confirmation(Packet *confirmation);

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = 1,
};
