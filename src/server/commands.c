#include "server.h"

#include <stdlib.h>

// TODO: need a better way to keep track of what should be free()d (probably just arena)

Packet command_list(char *path) {
    DIR *dirp = opendir(path);
    if(!dirp) {
        perror("opendir");
        return (Packet){0};
    }

    char *list = malloc(1);
    size_t size = 0;
    struct dirent *dir;
    while((dir = readdir(dirp)) != NULL) {
        size += strlen(dir->d_name) + 1;    // null terminated name
        list = realloc(list, size);
        // TODO: symlinks?
        strcat(list, dir->d_name);
        strcat(list, "\0");
    }
    closedir(dirp);

    return (Packet) {
        .raw = list,
        .size = size
    };
}

Packet command_metadata(char *path, size_t index) {
    DIR *dirp = opendir(path);
    if(!dirp) {
        perror("opendir");
        return (Packet){0};
    };

    struct dirent *dir;
    for(size_t i = 0; i < index - 1; i++)
        dir = readdir(dirp);
    dir = readdir(dirp);

    size_t size = strlen(dir->d_name) + 1;
    char *metadata = malloc(size); // need to alloc for usual command freeing
    strcpy(metadata, dir->d_name);

    closedir(dirp);
    return (Packet) {
        .raw = metadata,
        .size = size
    };
}
