#include "server.h"

#include <stdlib.h>

// TODO: need a better way to keep track of what should be free()d
// TODO: fix freeing problem, need to keep size to know what to free
PacketBody command_fetch_list(char *path) {
    DIR *dirp = opendir(path);
    if(!dirp) {
        fprintf(stderr, "failed to fetch list");
        return (PacketBody){0};
    }

    char **response = malloc(1 * sizeof(char *));
    struct dirent *dir;
    size_t amount;
    for(amount = 0; (dir = readdir(dirp)) != NULL; amount++) {
        response = realloc(response, (amount+1) * sizeof(char *));
        // TODO: symlinks?
        response[amount] = dir->d_name;
    }
    closedir(dirp);

    return (PacketBody) {
        .raw = response,
        .seg = amount
    };
}
