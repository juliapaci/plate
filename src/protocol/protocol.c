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

void respond(int client, RequestKind req, Packet *packet, size_t depth) {
    packet->header.size = body_size(packet->body, req);
    send(client, packet, sizeof(PacketHeader), 0);
    send(client, &packet->body.seg, sizeof(size_t), 0);
    // TODO: could serialise and deserialize double pointer for sending
    if(packet->body.seg == 0)
        send(client, packet->body.raw, packet->header.size, 0);
    else
        for(size_t i = 0; i < packet->body.seg; i++) {
            void *curr = ((void **)packet->body.raw)[i];
            send(client, curr, strlen(curr), 0);
        }

    if(depth > MAX_DEPTH)
        return;

    // acknowledgement for EDAC
    if(!EXPECT_ACK(req))
        return;

    // TODO: an actual acknowledgement packet
    // TODO: socket timeout but only for certain packets not all
    bool ack = false;
    recv(client, &ack, sizeof(ack), 0);
    if(!ack)
        // TODO: not sure if its better to keep the response data and resend it manually rather than doing some redundant computations
        respond(client, req, packet, depth + 1);
}

Packet request(int server, RequestKind req) {
    send(server, &req, sizeof(RequestKind), 0);

    Packet response;
    recv(server, &response.header, sizeof(PacketHeader), 0);
    recv(server, &response.body.seg, sizeof(size_t), 0);
    // response.body.raw = malloc(response.body.seg * sizeof(void *));
    // for(size_t i = 0; i < response.body.seg; i++) {
    //     ((void **)response.body.raw)[i] = malloc(sizeof(void *));
    // }

    response.body.raw = malloc(response.header.size);
    recv(server, response.body.raw, response.header.size, 0);

    // acknowledgement for EDAC
    if(!EXPECT_ACK(req))
        return response;
    const bool ack = true;
    send(server, &ack, sizeof(ack), 0);
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
