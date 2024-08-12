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

size_t body_size(PacketBody body, RequestKind req) {
    size_t size = 0;

    switch(req) {
        case LIST:
            for(size_t i = 0; i < body.seg; i++)
                size += strlen(((char **)body.raw)[i]) + 1; // null terminated
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

    // send data
    send(client_fd, packet, sizeof(PacketHeader), 0);
    send(client_fd, &packet->body.seg, sizeof(size_t), 0);
    // TODO: could serialise and deserialize double pointer for sending
    if(packet->body.seg == 0)
        send(client_fd, packet->body.raw, confirmation.body.size, 0);
    else
        for(size_t i = 0; i < packet->body.seg; i++) {
            void *curr = ((void **)packet->body.raw)[i];
            send(client_fd, curr, strlen(curr), 0);
        }
}

// TODO: actual error handling or propagate
// returns SERVER_PACKET on error
Packet request(int server_fd, RequestKind req) {
    // recieve confirmation
    send(server_fd, &req, sizeof(RequestKind), 0);
    Packet confirmation;
    recv(server_fd, &confirmation, sizeof(Packet), 0);
    if(!validate_confirmation(&confirmation)) {
        PRINT_RESPONSE(confirmation.header.res_kind);
        return SERVER_PACKET;
    }

    // recieve data
    Packet response;
    recv(server_fd, &response.header, sizeof(PacketHeader), 0);
    recv(server_fd, &response.body.seg, sizeof(size_t), 0);
    // response.body.raw = malloc(response.body.seg * sizeof(void *));
    // for(size_t i = 0; i < response.body.seg; i++) {
    //     ((void **)response.body.raw)[i] = malloc(sizeof(void *));
    // }

    response.body.raw = malloc(confirmation.body.size);
    recv(server_fd, response.body.raw, confirmation.body.size, 0);

    return response;
}

extern bool validate_handshake(Handshake *handshake);
extern bool validate_confirmation(Packet *confirmation);
extern void free_body(PacketBody *body);

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = 1,
};
