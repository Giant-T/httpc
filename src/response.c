#include "response.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "files.h"

void respond(Request *request, SOCKET client) {
    File file;
    bool worked = read_file(&file, request->path);

    if (!worked) {
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
    } else if (!strcmp(extension, "css")) {
        content_type = "Content-Type: text/css; charset=utf-8\n\n";
    } else {
        content_type = "Content-Type: text/plain; charset=utf-8\n\n";
    }
    response_len += strlen(content_type);
    response = realloc(response, response_len);
    strncat(response, content_type, response_len);

    response_len += file.size;
    response = realloc(response, response_len);
    strncat(response, file.content, response_len);

    int offset = 0;
    int sent;
    response_len = strlen(response);
    while (offset < response_len) {
        sent = send(client, response + offset, response_len - offset, 0);
        if (sent <= 0) break;
        offset += sent;
    }

    free(file.content);
    free(response);
}

void _handle_file_error(SOCKET client, char *path) {
    int err = errno;
    if (err == ENOENT) {
        respond_error(client, 404);
        log_err("file \"%s\" not found\n", path);
    } else {
        respond_error(client, 500);
        log_err("file operation failed with %d\n", err);
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
