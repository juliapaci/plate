#include "protocol.h"

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = 1,
};

bool validate_handshake(unsigned char *packet) {
    return memcmp(&HANDSHAKE_EXPECTED, packet, sizeof(Handshake)) == 0;
}
