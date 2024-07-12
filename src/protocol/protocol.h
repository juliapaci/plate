#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    HANDSHAKE,  // initial handshake
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
    LIST,       // lists servers root directory
} RequestKind;

// return codes for any server commands
typedef enum {
    SUCCESS,            // generic success
    FAILURE,            // generic error
    HANDSHAKE_SUCCESS,  // successfully validated the handshake
    HANDSHAKE_FAILURE,  // failed to validate the appropriate handshake
} ResponseKind;

// TODO: this or two seperate packets for requests and responses?
typedef struct {
    bool whence; // 0 - server, 1 - client
    union {
        RequestKind req_kind;
        ResponseKind res_kind;
    };
} PacketHeader;

typedef struct {
    PacketHeader header;
    void *body;
} Packet;

typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint8_t version;
} Handshake;

extern const Handshake HANDSHAKE_EXPECTED;

// TODO: not sure if this should be inlined
// if the handshake is valid all requests will be casted to a Packet
inline bool validate_handshake(unsigned char *packet) {
    return memcmp(&HANDSHAKE_EXPECTED, packet, sizeof(Handshake)) == 0;
}

#endif // __PROTOCOL_H__
