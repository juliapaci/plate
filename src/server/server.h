#ifndef __SERVER_H__
#define __SERVER_H__

#include <protocol.h>
#include <netinet/in.h>
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <stdbool.h>

// server options
typedef struct {
    // quality
    bool quality_variable;  // changing levels of quality
    size_t quality;         // severity of compression stuff

    size_t chunks_forward;  // max amount of chunks to load in advance
    size_t chunks_backward; // max amount of chunks to retroactively load
    size_t chunk_size;      // size of each chunk
} Options;

extern const Options OPTIONS_DEFAULT;

// server state
typedef struct {
    // net
    uint16_t port;

    // misc
    DIR *root;  // where the server root is located
} Server;

struct SharedState {
    int client;
    Server *server_state;
};

// meta
Options parse_options_base(FILE *stream);
Options parse_options(int argc, char **argv);

Server init_state(uint16_t port);
void init_server(Server *server);
void *_handle_client(void *arg);
void clean_state(Server *server);   // cleans up Server

// server commands/requests for client
Packet process_request(Packet *request, Server *server);

char **fetch_list(DIR *dir);  // Fetches the root directory contents

#endif // __SERVER_H__
