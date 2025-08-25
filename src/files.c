#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool read_file(File *file, const char *path) {
    FILE *fp = fopen(path, "r");
    file->extension = get_file_extension(path);

    if (fp == NULL) return false;

    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    rewind(fp);

    file->content = malloc(file->size + 1);
    memset(file->content, 0, file->size + 1);

    fread(file->content, 1, file->size, fp);

    fclose(fp);

    return true;
}

const char *get_file_extension(const char *path) {
    size_t itr = strlen(path);
    while (itr--) {
        if (path[itr] == '.') break;
    }
    return path + itr + 1;
}
