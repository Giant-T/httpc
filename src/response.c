#include "response.h"

#include <WS2tcpip.h>
#include <fileapi.h>
#include <handleapi.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winerror.h>

#include "print.h"

void respond(request_t *request, SOCKET client) {
    char *content = NULL;
    size_t content_len = _get_file_content(request->path, &content);

    if (content == NULL) {
        _handle_file_error(client, request->path);
    }

    // DO NOT FORGET TO UPDATE CHAR COUNT
    char *response = malloc(17);
    memset(response, 0, 17);
    strcpy_s(response, 17, "HTTP/1.1 200 OK\n");
    size_t response_len = strlen(response) + 1;

    char *extension = _get_file_extension(request->path);
    char *content_type;

    if (!strcmp(extension, "html")) {
        content_type = "Content-Type: text/html; charset=utf-8\n\n";
    } else {
        content_type = "Content-Type: text/plain; charset=utf-8\n\n";
    }
    response_len += strlen(content_type);
    response = realloc(response, response_len);
    strcat_s(response, response_len, content_type);

    response_len += content_len;
    response = realloc(response, response_len);
    strcat_s(response, response_len, content);

    int offset = 0;
    int sent;
    response_len = strlen(response);
    while (offset < response_len) {
        sent = send(client, response + offset, response_len - offset, 0);
        if (sent <= 0) break;
        offset += sent;
    }

    free(content);
    free(response);
}

size_t _get_file_content(char *path, char **content) {
    HANDLE file_handle = CreateFileA(
        path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    size_t file_size = GetFileSize(file_handle, NULL) + 1;

    *content = malloc(file_size);
    memset(*content, 0, file_size);
    DWORD read_bytes = 0;
    boolean did_read = ReadFile(file_handle, *content, file_size, &read_bytes, NULL);

    if (!did_read) {
        CloseHandle(file_handle);
        free(*content);
        content = NULL;
        return 0;
    }

    CloseHandle(file_handle);
    return file_size;
}

char *_get_file_extension(char *path) {
    size_t itr = strlen(path);
    while (itr--) {
        if (path[itr] == '.') break;
    }
    return path + itr + 1;
}

void _handle_file_error(SOCKET client, char *path) {
    DWORD err = GetLastError();
    if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
        respond_error(client, 404);
        print_err("file \"%s\" not found\n", path);
    } else {
        respond_error(client, 500);
        print_err("file operation failed with %ld\n", err);
    }
}

void respond_error(SOCKET client, uint16_t code) {
    char *response = malloc(23);
    uint32_t response_len = sprintf(response, "HTTP/1.1 %d ERROR\n\n", code);
    send(client, response, response_len, 0);
    free(response);
}
