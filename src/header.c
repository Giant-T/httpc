#include "header.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void headers_init(Headers* headers, size_t size) {
    headers->headers = malloc(headers->size * sizeof(char**));
    headers->len = 0;
    headers->size = headers->size;
}

void headers_free(Headers *headers) {
    while (headers->len--)
        free(headers->headers[headers->len]);

    free(headers->headers);
    headers->headers = NULL;
    headers->size = headers->len;
}

void headers_add(Headers* headers, const char* header) {
    if (headers->len == headers->size) {
        headers->size *= 2;
        headers->headers = realloc(headers->headers, headers->size * sizeof(char**));
    }
    size_t header_len = strlen(header) + 1;
    headers->headers[headers->len] = malloc(header_len);
    strncpy(headers->headers[headers->len], header, header_len);
    headers->len++;
}
