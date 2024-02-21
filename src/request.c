#include "request.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request_t parse_request(char* buf) {
    request_t request = _initialize_request(10);

    char* l_context = NULL;
    char* line = strtok_s(buf, "\n", &l_context);

    request.path = _parse_path(line);

    line = strtok_s(NULL, "\n", &l_context);

    while (line != NULL && line[0] != 0xd) {
        _insert_header(&request, line);

        line = strtok_s(NULL, "\n", &l_context);
    }

    return request;
}

char* _parse_path(char* line) {
    char* tok_context = NULL;
    strtok_s(line, " ", &tok_context);
    char* token = strtok_s(NULL, " ", &tok_context);
    size_t size = strlen(token) + 2;

    char* path = malloc(size);
    path[0] = '.';
    path[1] = '\0';
    strcat_s(path, size, token);

    if (path[size - 2] == '/') {
        path = realloc(path, size + 11);
        strcat_s(path, size + 11, "index.html");
    }

    return path;
}

request_t _initialize_request(uint32_t headers_size) {
    request_t request;
    request.headers = malloc(headers_size * sizeof(char**));
    request.headers_len = 0;
    request.headers_size = headers_size;
    request.path = NULL;

    return request;
}

void _insert_header(request_t* request, const char* header) {
    if (request->headers_len == request->headers_size) {
        request->headers_size *= 2;
        request->headers = realloc(request->headers, request->headers_size * sizeof(char**));
    }
    size_t header_len = strlen(header) + 1;
    request->headers[request->headers_len] = malloc(header_len);
    strcpy_s(request->headers[request->headers_len], header_len, header);
    request->headers_len++;
}

void free_request(request_t* request) {
    while (request->headers_len--) {
        free(request->headers[request->headers_len]);
    }
    free(request->headers);
    free(request->path);
    request->headers_size = request->headers_len;
    request->headers = NULL;
}
