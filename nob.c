#define NOBUILD_IMPLEMENTATION
#include "nob.h"

#define BUILD "build"
#define CFLAGS "-Wall", "-Wextra"
// TODO: not sure if "-I" is an linker flag?
#define LDFLAGS "-Lbuild", "-Isrc/protocol", "-lprotocol"
// TODO: find a way to automate making this
#define LDFLAGS_STRING "\"-Lbuild\", \"-Isrc/protocol\", \"-lprotocol\""

void build_protocol(void) {
    CMD("cc", CFLAGS, LDFLAGS, "-c", "-o", PATH(BUILD, "protocol.o"), PATH("src", "protocol", "protocol.c"));
    CMD("ar", "-rcs", PATH(BUILD, "libprotocol.a"), PATH(BUILD, "protocol.o"));
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
    CMD("cc", CFLAGS, LDFLAGS, "-o", PATH(BUILD, "server"), PATH("src", "server", "main.c"), PATH("src", "server", "server.c"));
}

void build_client(void) {
    CMD("cc", CFLAGS, LDFLAGS, "-o", PATH(BUILD, "client"), PATH("src", "client", "main.c"), PATH("src", "client", "client.c"));
}

// NOTE: very bare bones just so clangd can pick up on stuff
void create_compile_commands(void) {
    CMD("touch", "compile_commands.json");

    FILE *json = fopen("compile_commands.json", "w");
    fprintf(json,
        "["
        "{"
        "\"directory\": \"%s\","
        "\"arguments\": [/usr/bin/cc, %s],"
        "\"file\": N/A"
        "}"
        "]",
        GETCWD(),
        LDFLAGS_STRING
    );

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
