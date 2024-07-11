#ifndef __SERVER_H__
#define __SERVER_H__

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

// server state
typedef struct {
    DIR *living;    // where the server root is located
} Server;

extern const Options OPTIONS_DEFAULT;

// meta
Options parse_options_base(FILE *stream);
Options parse_options(int argc, char **argv);

Server init_state(void);
void init_server(uint16_t port);
void *_handle_client(void *arg);
void clean_state(Server *server);   // cleans up Server

// server commands/requests for client
char *fetch_list(DIR *dir);  // Fetches the directory contents seperated by a newline

#endif // __SERVER_H__
