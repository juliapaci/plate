#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

enum {
    METADATA,   // gets the metadata
    LOAD,       // loads a chunk
};

struct Packet {

};

typedef struct __attribute__((packed)) {
    uint16_t magic;
    uint8_t version;
} Handshake;

extern const Handshake HANDSHAKE_EXPECTED;

// TODO: not sure if this should be inlined
bool validate_handshake(unsigned char *packet);

#endif // __PROTOCOL_H__
