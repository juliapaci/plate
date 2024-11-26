#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// TODO: add possibility for encrypted transfers

#define VERSION (uint8_t)0

// TODO: properly manage endianness

// TODO: should make this configurable
#define MAX_DEPTH 5 // repitition for EDAC
#define MAX_SIZE 50 // (KB)

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

typedef struct __attribute__((packed)) {
    size_t size;    // total body size (including all segments)
    void *raw;      // pointer to data
} Packet;

typedef struct __attribute__((packed)) {
    RequestKind kind;
    size_t args;
} Request;

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

// send a variable length packet from the server
// depth for retransmission
void respond(int client, RequestKind req, Packet *packet, size_t depth);
// send a request from a client
Packet request(int server_fd, Request req);

// if the handshake is valid all requests will be casted to a Packet
inline bool validate_handshake(Handshake *handshake) {
    const Handshake expected_handshake = HANDSHAKE_EXPECTED;
    return memcmp(&expected_handshake, handshake, sizeof(Handshake)) == 0;
}

#endif // __PROTOCOL_H__
