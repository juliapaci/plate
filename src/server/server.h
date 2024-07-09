#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // server
    in_port_t port;

    // quality
    bool quality_variable;  // changing levels of quality
    size_t quality;         // severity of compression stuff

    size_t chunks_forward;  // max amount of chunks to load in advance
    size_t chunks_backward; // max amount of chunks to retroactively load
} Options;

const Options OPTIONS_DEFAULT = {
    .quality_variable = false,
    .quality = 0,
    .chunks_forward = 3,
    .chunks_backward = 2
};

Options parse_options_base(FILE *stream);
Options parse_options(int argc, char **argv);

int init_server_base(uint16_t port);
int init_server(Options *options);

#endif // __SERVER_H__
