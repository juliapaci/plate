#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PRINT_RESPONSE(response) fprintf(stderr, "%s\n", response_string(response));

typedef enum {
    HANDSHAKE,  // initial handshake
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
    LIST,       // lists servers root directory
    EXIT,       // close the client connection
} RequestKind;

// return codes for any server commands
// successes are odd and failures are even
typedef enum {
    SUCCESS,            // generic success
    FAILURE,            // generic error
    HANDSHAKE_SUCCESS,  // successfully validated the handshake
    HANDSHAKE_FAILURE,  // failed to validate the appropriate handshake
} ResponseKind;

// TODO: this or two seperate packets for requests and responses?
typedef struct __attribute__((packed)) {
    bool whence; // 0 - server, 1 - client
    union {
        RequestKind req_kind;
        ResponseKind res_kind;
    };
} PacketHeader;

typedef union __attribute__((packed)) {
    // METADATA
    char *metadata;
    // LIST
    char **list;
    // EXIT
    bool exit;
} Body;

typedef struct __attribute__((packed)) {
    PacketHeader header;
    Body body;
} Packet;

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

// TODO: not sure if this should be inlined
// if the handshake is valid all requests will be casted to a Packet
inline bool validate_handshake(unsigned char *packet) {
    return memcmp(&HANDSHAKE_EXPECTED, packet, sizeof(Handshake)) == 0;
}

#endif // __PROTOCOL_H__
