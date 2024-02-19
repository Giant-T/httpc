#include "response.h"

#include <WS2tcpip.h>
#include <fileapi.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winerror.h>

void respond(request_t *request, SOCKET client) {
    HANDLE file_handle = CreateFileA(
        request->url, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            respond_error(client, 404);
            fprintf(stderr, "ERROR: file \"%s\" not found\n", request->url);
        } else {
            respond_error(client, 500);
            fprintf(stderr, "ERROR: CreateFileA failed with %ld\n", err);
        }
        return;
    }

    // DO NOT FORGET TO UPDATE CHAR COUNT
    char *response = malloc(43);
    strcpy_s(response, 43, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
    size_t response_len = strlen(response);

    size_t file_size = GetFileSize(file_handle, NULL);

    char *content = malloc(file_size + 1);
    ReadFile(file_handle, content, file_size, NULL, NULL);

    response_len += file_size + 1;
    response = realloc(response, response_len);
    strcat_s(response, response_len, content);

    size_t offset = 0;
    size_t sent;
    while (offset < response_len) {
        sent = send(client, response + offset, response_len - offset, 0);
        if (sent <= 0) break;
        offset += sent;
    }

    free(content);
    free(response);
}

void respond_error(SOCKET client, uint16_t code) {
    char *response = malloc(23);
    uint32_t response_len = sprintf(response, "HTTP/1.1 %d ERROR\n\n", code);
    send(client, response, response_len, 0);
    free(response);
}
