#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // quality
    bool quality_variable;  // changing levels of quality
    size_t quality;         // severity of compression stuff

    size_t chunks_forward;  // max amount of chunks to load in advance
    size_t chunks_backward; // max amount of chunks to retroactively load
} Options;

extern const Options OPTIONS_DEFAULT;

Options parse_options_base(FILE *stream);
Options parse_options(int argc, char **argv);

void init_server(uint16_t port);

#endif // __SERVER_H__
