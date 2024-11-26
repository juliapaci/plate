#include "protocol.h"

#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

// TODO: X macros
const char *request_string_direct(RequestKind request) {
    return (const char *[]){"handshake", "metadata", "load", "list", "exit"}[request];
}

RequestKind string_request_direct(char *request) {
    if(strcmp(request, "list") == 0)
        return LIST;
    if(strcmp(request, "metadata") == 0)
        return METADATA;
    else if(strcmp(request, "exit") == 0)
        return EXIT;

    return EXIT;
}

void respond(int client, RequestKind req, Packet *packet, size_t depth) {
    send(client, &packet->size, sizeof(size_t), 0);
    send(client, packet->raw, packet->size, 0);

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

Packet request(int server, Request req) {
    send(server, &req, sizeof(Request), 0);

    Packet response;
    recv(server, &response, sizeof(size_t), 0);
    response.raw = malloc(response.size);
    recv(server, response.raw, response.size, 0);

    // acknowledgement for EDAC
    if(!EXPECT_ACK(req.kind))
        return response;
    const bool ack = true;
    send(server, &ack, sizeof(ack), 0);
    return response;
}

extern bool validate_handshake(Handshake *handshake);
extern bool validate_confirmation(Packet *confirmation);

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = VERSION,
};;
