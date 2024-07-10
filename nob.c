#define NOBUILD_IMPLEMENTATION
#include "nob.h"

#define BUILD "build"
#ifdef DEBUG
#define CFLAGS "-Wall", "-Wextra", "-ggdb"
#else
#define CFLAGS "-Wall", "-Wextra"
#endif // DEBUG

void build_server(void) {
    // CMD("mkdir", "-p", "server");

    // TODO: real incremental compilation
    // FOREACH_FILE_IN_DIR(src, PATH("src", "server"), {
    //     if(ENDS_WITH(src, ".c"))
    //         CMD(
    //             "cc", CFLAGS,
    //             "-o", PATH(BUILD, "server", CONCAT(NOEXT(src), ".o")),
    //             "-c", src
    //         );
    // });
    CMD("cc", CFLAGS, "-o", PATH(BUILD, "server"), PATH("src", "server", "main.c"), PATH("src", "server", "server.c"));
}

void build_client(void) {
    CMD("cc", CFLAGS, "-o", PATH(BUILD, "client"), PATH("src", "client", "main.c"), PATH("src", "client", "client.c"));
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);

    CMD("mkdir", "-p", BUILD);
    build_server();
    build_client();

    return 0;
}
