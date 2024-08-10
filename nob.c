#define NOBUILD_IMPLEMENTATION
#include "nob.h"
#include <string.h>

#define BUILD "build"
#define CFLAGS "-Wall", "-Wextra", "-ggdb"
// TODO: not sure if "-I" is an linker flag?
#define LDFLAGS "-Lbuild", "-Isrc/protocol", "-lprotocol"
#define LDFLAGS_DELIM "\", \""

void build_protocol(void) {
    CMD("cc", CFLAGS, LDFLAGS, "-c", "-o", PATH(BUILD, "protocol.o"), PATH("src", "protocol", "protocol.c"));
    CMD("ar", "rcs", PATH(BUILD, "libprotocol.a"), PATH(BUILD, "protocol.o"));
}

void build_server(void) {
    // CMD("mkdir", "-p", PATH(BUILD, "server"));

    // TODO: real incremental compilation
    // FOREACH_FILE_IN_DIR(src, PATH("src", "server"), {
    //     if(ENDS_WITH(src, ".c"))
    //         CMD(
    //             "cc", CFLAGS,
    //             "-o", PATH(BUILD, "server", CONCAT(NOEXT(src), ".o")),
    //             "-c", src
    //         );
    // });
    CMD("cc", CFLAGS,  "-o", PATH(BUILD, "server"), PATH("src", "server", "main.c"), PATH("src", "server", "server.c"), LDFLAGS);
}

void build_client(void) {
    CMD("cc", CFLAGS, "-o", PATH(BUILD, "client"), PATH("src", "client", "main.c"), PATH("src", "client", "client.c"), LDFLAGS);
}

// NOTE: very bare bones just so clangd can pick up on stuff
void create_compile_commands(void) {
    CMD("touch", "compile_commands.json");
    FILE *json = fopen("compile_commands.json", "w");

    const char *ldflags_array[] = {LDFLAGS};
    const size_t ldflags_amount = sizeof(ldflags_array)/sizeof(char *);
    size_t ldflags_size = 0;
    for(size_t i = 0; i < ldflags_amount; i++)
        ldflags_size += strlen(ldflags_array[i]) + strlen(LDFLAGS_DELIM);

    char *ldflags_args = malloc(ldflags_size);
    ldflags_args[0] = '\0';
    for(size_t i = 0; i < ldflags_amount; i++) {
        ldflags_args = strcat(ldflags_args, LDFLAGS_DELIM);
        ldflags_args = strcat(ldflags_args, ldflags_array[i]);
    }

    fprintf(json,
        "["                                     "\n"
        "\t"    "{"                             "\n"
        "\t\t"      "\"directory\": \"%s\","    "\n"
        // stupid formatting but works cause order of appending LDFLAGS_DELIM
        "\t\t"      "\"arguments\": [\"/usr/bin/cc%s\"],"   "\n"
        "\t\t"      "\"file\": \"N/A\""         "\n"
        "\t"    "}"                             "\n"
        "]",
        GETCWD(),
        ldflags_args
    );

    free(ldflags_args);
    fclose(json);
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);

    CMD("mkdir", "-p", BUILD);
    INFO("building protocol"); build_protocol();
    INFO("building server"); build_server();
    INFO("building client"); build_client();
    INFO("creating \"compile_commands.json\""); create_compile_commands();

    return 0;
}
