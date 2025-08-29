#include "files.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_PATH "www"

char* concat_paths(const char* path1, const char* path2) {
    size_t result_len = 1;
    size_t path1_len = strlen(path1);
    result_len += path1_len;
    bool path1_ends_with_slash = path1[path1_len - 1] == '/';
    if (!path1_ends_with_slash) result_len++;

    if (path2[0] == '.') path2++;
    if (path2[0] == '/') path2++;
    size_t path2_len = strlen(path2);
    result_len += path2_len;

    char* result_path = malloc(result_len);
    memset(result_path, 0, result_len);
    strncpy(result_path, path1, path1_len);
    if (!path1_ends_with_slash) result_path[path1_len] = '/';
    strncat(result_path, path2, path2_len);

    return result_path;
}

bool read_file(File *file, const char *path) {
    char* complete_path = concat_paths(BASE_PATH, path);

    FILE *fp = fopen(complete_path, "r");
    file->extension = get_file_extension(path);
    free(complete_path);

    if (fp == NULL) return false;

    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    rewind(fp);

    file->content = malloc(file->size + 1);
    memset(file->content, 0, file->size + 1);

    uint64_t _ = fread(file->content, 1, file->size, fp);

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
