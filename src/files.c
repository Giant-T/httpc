#include "files.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/openat2.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BASE_PATH "www"

bool read_file(File *file, const char *path) {
    int dir = open(BASE_PATH, O_DIRECTORY | O_RDONLY);

    struct open_how how = {
        .flags = O_RDONLY,
        .resolve = RESOLVE_BENEATH,
    };
    long fd = syscall(SYS_openat2, dir, path, &how, sizeof(struct open_how));
    file->extension = get_file_extension(path);

    if (fd == -1) return false;

    FILE* fp = fdopen(fd, "r");

    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    rewind(fp);

    file->content = malloc(file->size + 1);
    memset(file->content, 0, file->size + 1);

    uint64_t _ = fread(file->content, 1, file->size, fp);

    fclose(fp);
    close(fd);
    close(dir);

    return true;
}

const char *get_file_extension(const char *path) {
    size_t itr = strlen(path);
    while (itr--) {
        if (path[itr] == '.') break;
    }
    return path + itr + 1;
}
