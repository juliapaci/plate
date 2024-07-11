#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
    LIST,       // lists servers root directory
} PacketKind;

typedef enum {
    HANDSHAKE_SUCCESS,  // successfully validated the handshake
    HANDSHAKE_FAILURE   // failed to validate the appropriate handshake
} ErrorKind;

typedef struct {
    PacketKind kind;
} Packet;

typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint8_t version;
} Handshake;

extern const Handshake HANDSHAKE_EXPECTED;

// TODO: not sure if this should be inlined
inline bool validate_handshake(unsigned char *packet) {
    return memcmp(&HANDSHAKE_EXPECTED, packet, sizeof(Handshake)) == 0;
}

#endif // __PROTOCOL_H__
