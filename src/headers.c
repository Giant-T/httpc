#include "headers.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split_header(char* str) {
    char** splits = malloc(2 * sizeof(char**));

    splits[0] = str;

    while (*str != ':') str++;
    *str = '\0';

    char* last_split = str + 1;
    while (isspace(*last_split)) last_split++;
    splits[1] = last_split;

    return splits;
}

void header_init(Header* self, const char* key, const char* value) {
    size_t key_len = strlen(key) + 1;
    self->key = malloc(key_len);
    strncpy(self->key, key, key_len);

    size_t value_len = strlen(value) + 1;
    self->value = malloc(value_len);
    strncpy(self->value, value, value_len);
}

char* header_to_string(const Header* self) {
    size_t key_len = strlen(self->key);
    size_t value_len = strlen(self->value);
    size_t str_len = key_len + value_len + 4;
    char* str = malloc(str_len);
    memset(str, 0, str_len);
    sprintf(str, "%s: %s\n", self->key, self->value);
    return str;
}

void header_free(Header* self) {
    free(self->key);
    self->key = NULL;
    free(self->value);
    self->value = NULL;
}

void headers_init(Headers* self, size_t size) {
    self->headers = malloc(self->size * sizeof(Header));
    self->len = 0;
    self->size = self->size;
}

void headers_free(Headers* self) {
    while (self->len--)
        header_free(&self->headers[self->len]);

    free(self->headers);
    self->headers = NULL;
    self->size = self->len = 0;
}

void headers_add(Headers* self, const char* key, const char* value) {
    if (self->len == self->size) {
        self->size *= 2;
        self->headers = realloc(self->headers, self->size * sizeof(Header));
    }

    header_init(&self->headers[self->len], key, value);
    self->len++;
}

Header* headers_get(Headers* self, const char* key) {
    for (size_t idx = 0; idx < self->len; idx++) {
        if (strcmp(self->headers[idx].key, key) == 0)
            return &self->headers[idx];
    }

    return NULL;
}

char* headers_to_string(Headers* self) {
    char** lines = malloc(self->len * sizeof(char**));

    size_t str_len = 2; // \n\0 at the end

    for (size_t idx = 0; idx < self->len; idx++) {
        lines[idx] = header_to_string(&self->headers[idx]);
        str_len += strlen(lines[idx]);
    }

    char* str = malloc(str_len);
    memset(str, 0, str_len);

    for (size_t idx = 0; idx < self->len; idx++) {
        strcat(str, lines[idx]);
        free(lines[idx]);
    }

    str[str_len - 2] = '\n';

    return str;
}
