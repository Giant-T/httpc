#include "response.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"

void respond(request_t *request, SOCKET client) {
    char *content = NULL;
    size_t content_len = _get_file_content(request->path, &content);

    if (content == NULL) {
        _handle_file_error(client, request->path);
        return;
    }

    // DO NOT FORGET TO UPDATE CHAR COUNT
    char *response = malloc(17);
    memset(response, 0, 17);
    strncpy(response, "HTTP/1.1 200 OK\n", 17);
    size_t response_len = strlen(response) + 1;

    char *extension = _get_file_extension(request->path);
    char *content_type;

    if (!strcmp(extension, "html")) {
        content_type = "Content-Type: text/html; charset=utf-8\n\n";
    } else {
        content_type = "Content-Type: text/plain; charset=utf-8\n\n";
    }
    response_len += strlen(content_type);
    response = realloc(response, response_len);
    strncat(response, content_type, response_len);

    response_len += content_len;
    response = realloc(response, response_len);
    strncat(response, content, response_len);

    int offset = 0;
    int sent;
    response_len = strlen(response);
    while (offset < response_len) {
        sent = send(client, response + offset, response_len - offset, 0);
        if (sent <= 0) break;
        offset += sent;
    }

    free(content);
    free(response);
}

size_t _get_file_content(char *path, char **content) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    *content = malloc(file_size);
    memset(*content, 0, file_size);
    bool did_read = fread(*content, file_size, file_size, file);

    if (!did_read) {
        fclose(file);
        free(*content);
        content = NULL;
        return 0;
    }

    fclose(file);
    return file_size;
}

void _handle_file_error(SOCKET client, char *path) {
    int err = errno;
    if (err == ENOENT) {
        respond_error(client, 404);
        print_err("file \"%s\" not found\n", path);
    } else {
        respond_error(client, 500);
        print_err("file operation failed with %d\n", err);
    }
}

char *_get_file_extension(char *path) {
    size_t itr = strlen(path);
    while (itr--) {
        if (path[itr] == '.') break;
    }
    return path + itr + 1;
}

void respond_error(SOCKET client, uint16_t code) {
    char *response = malloc(23);
    uint32_t response_len = sprintf(response, "HTTP/1.1 %d ERROR\n\n", code);
    send(client, response, response_len, 0);
    free(response);
}
