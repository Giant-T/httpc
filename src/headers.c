#include "headers.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Fix should use a function that works like split_once
char** split_header(char* str) {
    char* l_context = NULL;

    char** splits = malloc(2 * sizeof(char**));

    char* split = strtok_r(str, ":", &l_context);
    splits[0] = split;
    split = strtok_r(NULL, ":", &l_context);
    while (isspace(*split)) split++;
    splits[1] = split;

    return splits;
}

void header_init(Header* header, const char* key, const char* value) {
    size_t key_len = strlen(key) + 1;
    header->key = malloc(key_len);
    strncpy(header->key, key, key_len);

    size_t value_len = strlen(key) + 1;
    header->value = malloc(value_len);
    strncpy(header->value, value, value_len);
}

char* header_to_string(const Header* header) {
    size_t key_len = strlen(header->key);
    size_t value_len = strlen(header->value);
    size_t str_len = key_len + value_len + 4;
    char* str = malloc(str_len);
    memset(str, 0, str_len);
    sprintf(str, "%s: %s\n", header->key, header->value);
    return str;
}

void header_free(Header* header) {
    free(header->key);
    header->key = NULL;
    free(header->value);
    header->value = NULL;
}

void headers_init(Headers* headers, size_t size) {
    headers->headers = malloc(headers->size * sizeof(Header));
    headers->len = 0;
    headers->size = headers->size;
}

void headers_free(Headers* headers) {
    while (headers->len--)
        header_free(&headers->headers[headers->len]);

    free(headers->headers);
    headers->headers = NULL;
    headers->size = headers->len;
}

void headers_add(Headers* headers, const char* key, const char* value) {
    if (headers->len == headers->size) {
        headers->size *= 2;
        headers->headers = realloc(headers->headers, headers->size * sizeof(Header));
    }

    header_init(&headers->headers[headers->len], key, value);
    headers->len++;
}
