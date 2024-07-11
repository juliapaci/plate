#include "protocol.h"
#include <string.h>

extern bool validate_handshake(unsigned char *packet);

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = 1,
};
