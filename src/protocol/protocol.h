#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 1024 * 1

#define PRINT_RESPONSE(response) fprintf(stderr, "%s\n", response_string(response));

// valid requests for the client to send to the server
typedef enum {
    HANDSHAKE,  // initial handshake
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
    LIST,       // lists servers root directory
    EXIT,       // close the client connection
} RequestKind;

// return codes for any server commands
typedef enum {
    PRELUDE,            // confirmation/prelude information for an incoming packet e.g. expected size
    SUCCESS,            // generic success
    FAILURE,            // generic error
    HANDSHAKE_SUCCESS,  // successfully validated the handshake
    HANDSHAKE_FAILURE,  // failed to validate the appropriate handshake
} ResponseKind;

enum PacketWhence {
    SERVER,
    CLIENT
};

// TODO: maybe shouldnt have prelude res_kidne by default.
//      if we chage it we have to change it in confirmation validation aswell
#define SERVER_PACKET (Packet) {    \
    .header = (PacketHeader) {      \
        .whence = SERVER,           \
        .res_kind = PRELUDE         \
    }                               \
}

#define CLIENT_PACKET (Packet) {    \
    .header = (PacketHeader) {      \
        .whence = CLIENT,           \
    }                               \
}

// TODO: this or two seperate packets for requests and responses?
typedef struct __attribute__((packed)) {
    enum PacketWhence whence;
    // TODO: not sure if i have to explicitely pack here
    union __attribute__((packed)) {
        RequestKind req_kind;
        ResponseKind res_kind;
    };
} PacketHeader;

// references RequestKind
typedef struct __attribute__((packed)) {
    // TODO: could use segs instead of prelude size
    union __attribute__((packed)) {
        struct {
            void *raw;  // pointer to data
            size_t seg; // segment amount e.g. double pointer count
        };
        size_t size; // prelude
    };
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

// send a variable length packet from the server (confirm/prelude sending)
void respond(int client_fd, RequestKind req, Packet *packet);
// send a request from a client (confirm/prelude expecting)
Packet request(int server_fd, RequestKind req);

// if the handshake is valid all requests will be casted to a Packet
inline bool validate_handshake(Handshake *handshake) {
    const Handshake expected_handshake = HANDSHAKE_EXPECTED;
    return memcmp(&expected_handshake, handshake, sizeof(Handshake)) == 0;
}

// if the confirmation is verified requests can proceed as usual
inline bool validate_confirmation(Packet *confirmation) {
    const Packet expected_confirmation = SERVER_PACKET;
    return memcmp(&expected_confirmation, confirmation, sizeof(PacketHeader)) == 0;
}

inline void free_body(PacketBody *body) {
    for(size_t i = 0; i < body->seg; i++)
        free(((void **)body->raw)[i]);
    free(body->raw);

    *body = (PacketBody){0};
}

#endif // __PROTOCOL_H__
