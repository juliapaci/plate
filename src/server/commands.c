#include "server.h"

#include <stdlib.h>

// TODO: need a better way to keep track of what should be free()d (probably just arena)

Packet command_fetch_list(char *path) {
    DIR *dirp = opendir(path);
    if(!dirp) {
        fprintf(stderr, "failed to fetch list");
        return (Packet){0};
    }

    char *list = malloc(1);
    size_t size = 0;
    struct dirent *dir;
    for(size_t i = 0; (dir = readdir(dirp)) != NULL; i++) {
        size += strlen(dir->d_name) + 1; // null terminated
        list = realloc(list, size);
        // TODO: symlinks?
        strcat(list, dir->d_name);
        strcat(list, "\0");
    }
    closedir(dirp);

    printf("sizesize %ld\n", size);
    return (Packet) {
        .raw = list,
        .size = size
    };
}
