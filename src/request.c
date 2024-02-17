#include "request.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request_t parse_request(char* buf, uint32_t len) {
    request_t request = _initialize_request(10);

    char* l_context = NULL;
    char* line = strtok_s(buf, "\n", &l_context);

    char* tok_context = NULL;
    strtok_s(line, " ", &tok_context);
    request.file = strtok_s(NULL, " ", &tok_context);

    size_t size = strlen(request.file);
    if (request.file[size - 1] == '/') {  // Replaces the file with index if url ends with "/"
        request.file = "/index.html";
    }

    line = strtok_s(NULL, "\n", &l_context);

    while (line != NULL && line[0] != 0xd) {
        _insert_header(&request, line);

        line = strtok_s(NULL, "\n", &l_context);
    }

    return request;
}

request_t _initialize_request(uint32_t headers_size) {
    request_t request;
    request.headers = malloc(headers_size * sizeof(char**));
    request.headers_len = 0;
    request.headers_size = headers_size;

    return request;
}

void _insert_header(request_t* request, char* header) {
    if (request->headers_len == request->headers_size) {
        request->headers_size *= 2;
        request->headers = realloc(request->headers, request->headers_size * sizeof(char**));
    }
    request->headers[request->headers_len++] = header;
}

void free_request(request_t* request) {
    request->headers_size = request->headers_len = 0;
    free(request->headers);
}
