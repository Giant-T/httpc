#include "request.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"
#include "log.h"

Request parse_request(char* buf) {
    Request request;
    request_init(&request, 10);

    char* l_context = NULL;
    char* line = strtok_r(buf, "\n", &l_context);

    request.path = _parse_path(line);

    line = strtok_r(NULL, "\n", &l_context);

    while (line != NULL && line[0] != 0xd) {
        char** splits = split_header(line);
        headers_add(&request.headers, splits[0], splits[1]);
        free(splits);

        char* header_str = header_to_string(&request.headers.headers[request.headers.len - 1]);
        log_info("%s", header_str);
        free(header_str);

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

void request_init(Request* request, uint32_t headers_size) {
    headers_init(&request->headers, headers_size);
    request->path = NULL;
}

void request_free(Request* request) {
    headers_free(&request->headers);
    free(request->path);
    request->path = NULL;
}
