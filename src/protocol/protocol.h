#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PRINT_RESPONSE(response) fprintf(stderr, "%s\n", response_string(response));

#define MAX_DEPTH 5 // should make this configurable

// valid requests for the client to send to the server
typedef enum {
    HANDSHAKE,  // initial handshake
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
    LIST,       // lists servers root directory
    EXIT,       // close the client connection
} RequestKind;

// should use reflection cause this dependes on meta ordering
// should the server expect confirmation/acknowledgement that the request sent correctly without issue
// dont depend on TCP
#define EXPECT_ACK(req) (bool []){0,0,1,0,0}[req]

// return codes for any server commands
// TODO: more than simply SUCCESS and FAILURE arent necessary cause we progress linearly so we can always determine in what stage the failure should be interpreted as and stuff
typedef enum {
    SUCCESS,            // generic success
    FAILURE,            // generic error
    HANDSHAKE_SUCCESS,  // successfully validated the handshake
    HANDSHAKE_FAILURE,  // failed to validate the appropriate handshake
} ResponseKind;

enum PacketWhence {
    SERVER,
    CLIENT
};

// TODO: constants instead of macros?
#define SERVER_PACKET (Packet) {    \
    .header = (PacketHeader) {      \
        .whence = SERVER,           \
    }                               \
}

#define CLIENT_PACKET (Packet) {    \
    .header = (PacketHeader) {      \
        .whence = CLIENT,           \
    }                               \
}

// TODO: this or two seperate packets for requests and responses?
// TODO: should be some basic error detecting, maybe just through a checksum in there but we dont need it for all packets just for the EXPECT_CONFIRM or important data ones
typedef struct __attribute__((packed)) {
    enum PacketWhence whence;
    // TODO: not sure if i have to explicitely pack here
    union __attribute__((packed)) {
        RequestKind req_kind;
        ResponseKind res_kind;
    };
    size_t size; // body size
} PacketHeader;

// references RequestKind
typedef struct __attribute__((packed)) {
    size_t seg; // segment amount e.g. double pointer count
    void *raw;  // pointer to data
} PacketBody;

typedef struct __attribute__((packed)) {
    PacketHeader header;
    PacketBody body;
} Packet;

// TODO: make Handshake into a custom Packet as is Prelude
typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint8_t version;
} Handshake;

extern const Handshake HANDSHAKE_EXPECTED;

// serialization
// TODO: automated serialization
// directly serializes the variants' names into strings or "unknown"
const char *request_string_direct(RequestKind request);
// string -> request or EXIT
RequestKind string_request_direct(char *request);

// converts a ResponseKind to a message
// NOTE: only implements error responses or "success"
const char *response_string(ResponseKind response);

// size of `body`
size_t body_size(PacketBody body, RequestKind req);

// send a variable length packet from the server
// depth for retransmission
void respond(int client, RequestKind req, Packet *packet, size_t depth);
// send a request from a client
Packet request(int server_fd, RequestKind req);

// if the handshake is valid all requests will be casted to a Packet
inline bool validate_handshake(Handshake *handshake) {
    const Handshake expected_handshake = HANDSHAKE_EXPECTED;
    return memcmp(&expected_handshake, handshake, sizeof(Handshake)) == 0;
}

inline void free_body(PacketBody *body) {
    for(size_t i = 0; i < body->seg; i++)
        free(((void **)body->raw)[i]);
    free(body->raw);

    *body = (PacketBody){0};
}

#endif // __PROTOCOL_H__
