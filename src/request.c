#include "request.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Request parse_request(char* buf) {
    Request request = _initialize_request(10);

    char* l_context = NULL;
    char* line = strtok_r(buf, "\n", &l_context);

    request.path = _parse_path(line);

    line = strtok_r(NULL, "\n", &l_context);

    while (line != NULL && line[0] != 0xd) {
        _insert_header(&request, line);

        line = strtok_r(NULL, "\n", &l_context);
    }

    return request;
}

char* _parse_path(char* line) {
    char* tok_context = NULL;
    strtok_r(line, " ", &tok_context);
    char* token = strtok_r(NULL, " ", &tok_context);
    size_t size = strlen(token) + 2;

    char* path = malloc(size);
    path[0] = '.';
    path[1] = '\0';
    strncat(path, token, size);

    if (path[size - 2] == '/') {
        path = realloc(path, size + 11);
        strncat(path, "index.html", size + 11);
    }

    return path;
}

Request _initialize_request(uint32_t headers_size) {
    Request request;
    request.headers = malloc(headers_size * sizeof(char**));
    request.headers_len = 0;
    request.headers_size = headers_size;
    request.path = NULL;

    return request;
}

void _insert_header(Request* request, const char* header) {
    if (request->headers_len == request->headers_size) {
        request->headers_size *= 2;
        request->headers = realloc(request->headers, request->headers_size * sizeof(char**));
    }
    size_t header_len = strlen(header) + 1;
    request->headers[request->headers_len] = malloc(header_len);
    strncpy(request->headers[request->headers_len], header, header_len);
    request->headers_len++;
}

void free_request(Request* request) {
    while (request->headers_len--) {
        free(request->headers[request->headers_len]);
    }
    free(request->headers);
    free(request->path);
    request->headers_size = request->headers_len;
    request->headers = NULL;
}
