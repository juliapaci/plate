#include "protocol.h"
#include <string.h>
#include <sys/socket.h>

const char *request_string_direct(RequestKind request) {
    char *string;
    switch(request) {
        case HANDSHAKE:
            string = "handshake";
            break;
        case METADATA:
            string = "metadata";
            break;
        case LOAD:
            string = "load";
            break;
        case LIST:
            string = "list";
            break;
        case EXIT:
            string = "exit";
            break;
        default:
            string = "unknown";
            break;
    }

    return string;
}

RequestKind string_request_direct(char *request) {
    if(strcmp(request, "list") == 0)
        return LIST;
    else if(strcmp(request, "exit") == 0)
        return EXIT;

    return EXIT;
}

const char *response_string(ResponseKind response) {
    char *string;
    switch(response) {
        case FAILURE:
            string = "error failed";
            break;
        case HANDSHAKE_FAILURE:
            string = "failed to verify the handshake";
            break;
        default:
            string = "success";
            break;
    }

    return string;
}

void send_packet(int fd, RequestKind req, Packet *packet) {
    Packet response = {
        .header = packet->header,
        .size = 2 * sizeof(char *),
    };
    for(size_t i = 0; i < response.size/sizeof(char *); i++) {
        memcpy(response.raw + i*sizeof(packet->body.list[i]), packet->body.list[i], strlen(packet->body.list[i]));
    }

    send(fd, &response, PACKET_SIZE(response), 0);
}

extern bool validate_handshake(unsigned char *packet);

// NOTE: some fields like version can vary
const Handshake HANDSHAKE_EXPECTED = {
    .magic = ('P' << 8) | 'T',
    .version = 1,
};
