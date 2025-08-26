#include "response.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"
#include "log.h"
#include "files.h"

void respond(Request *request, Socket client) {
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

    Headers headers;
    headers_init(&headers, 10);

    if (!strcmp(file.extension, "html"))
        headers_add(&headers, "Content-Type", "text/html; charset=utf-8");
    else if (!strcmp(file.extension, "css"))
        headers_add(&headers, "Content-Type", "text/css; charset=utf-8");
    else if (!strcmp(file.extension, "png"))
        headers_add(&headers, "Content-Type", "image/png");
    else
        headers_add(&headers, "Content-Type", "text/plain; charset=utf-8");

    char file_size[20] = {0};
    snprintf(file_size, 20, "%ld", file.size);
    headers_add(&headers, "Content-Length", file_size);

    char* headers_str = headers_to_string(&headers);
    headers_free(&headers);

    response_len += strlen(headers_str);
    response = realloc(response, response_len);
    strncat(response, headers_str, response_len);
    free(headers_str);

    size_t end_idx = response_len - 1;
    response_len += file.size;
    response = realloc(response, response_len);
    memcpy(&response[end_idx], file.content, file.size);
    // strncat(response, file.content, response_len);

    int offset = 0;
    while (offset < response_len) {
        int sent = send(client, response + offset, response_len - offset, 0);
        if (sent <= 0) break;
        offset += sent;
    }

    free(file.content);
    free(response);
}

void _handle_file_error(Socket client, char *path) {
    int err = errno;
    if (err == ENOENT) {
        respond_error(client, 404);
        log_err("file \"%s\" not found\n", path);
    } else {
        respond_error(client, 500);
        log_err("file operation failed with %d\n", err);
    }
}

void respond_error(Socket client, uint16_t code) {
    char *response = malloc(23);
    uint32_t response_len = sprintf(response, "HTTP/1.1 %d ERROR\n\n", code);
    send(client, response, response_len, 0);
    free(response);
}
