#include "server.h"

#include <WS2tcpip.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>

void _start_wsa(void) {
    uint16_t version_requested = MAKEWORD(2, 2);
    WSADATA wsa_data;
    int32_t err = WSAStartup(version_requested, &wsa_data);

    if (err != 0) {
        fprintf(stderr, "ERROR: WSAStartup failed with error %d.\n", err);
        exit(EXIT_FAILURE);
    }
}

struct addrinfo *_get_addr(int32_t port) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char port_buf[10];
    sprintf(port_buf, "%d", port);

    int32_t err = getaddrinfo(NULL, port_buf, &hints, &res);

    if (err != 0) {
        fprintf(
            stderr, "ERROR: getaddrinfo failed with error %d\n",
            WSAGetLastError()
        );
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return res;
}

SOCKET _get_socket(struct addrinfo *addr) {
    SOCKET sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sfd == INVALID_SOCKET) {
        fprintf(
            stderr, "ERROR: socket failed with error %d\n", WSAGetLastError()
        );
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return sfd;
}

void _bind_socket(SOCKET sfd, struct addrinfo *addr) {
    int32_t err = bind(sfd, addr->ai_addr, addr->ai_addrlen);

    if (err != 0) {
        fprintf(
            stderr, "ERROR: bind failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void _listen_on_socket(SOCKET sfd) {
    if (listen(sfd, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(
            stderr, "ERROR: listen failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void _print_addr(struct sockaddr_storage *addr) {
    if (addr->ss_family == AF_INET) {
        char peer_name[INET_ADDRSTRLEN];
        inet_ntop(
            AF_INET, &((struct sockaddr_in *)addr)->sin_addr, peer_name,
            INET_ADDRSTRLEN
        );
        printf("REQUEST: from %s\n", peer_name);
    } else {
        char peer_name[INET6_ADDRSTRLEN];
        inet_ntop(
            AF_INET6, &((struct sockaddr_in6 *)addr)->sin6_addr, peer_name,
            INET_ADDRSTRLEN
        );
        printf("REQUEST: from %s\n", peer_name);
    }
}

void _handle_client(void *arg) {
    client_t *client = (client_t *)arg;

    char buf[1024];
    int32_t buflen = 1024;

    _print_addr(&client->addr);

    while (1) {
        int n = recv(client->socket, buf, buflen, 0);
        if (n > 0 && buf[n - 1] == '\n') {
            char response[] = "HTTP/1.1 200 OK\ncontent-type: text/html\n\nTEST";
            send(client->socket, response, strlen(response), 0);
            break;
        } else if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                printf("REQUEST: TIMED OUT\n");
                char response[] = "HTTP/1.1 408 Request Timeout\nConnection: close";
                send(client->socket, response, strlen(response), 0);
            } else {
                fprintf(stderr, "ERROR: recv failed with %d\n", WSAGetLastError());
            }

            break;
        }
    }

    closesocket(client->socket);
    free(client);
}

void _accept_connection(server_t *server) {
    client_t *client = malloc(sizeof(client_t));
    int32_t addr_len = sizeof(client->addr);
    client->socket = accept(server->socket, (struct sockaddr *)&client->addr, &addr_len);

    if (client->socket == INVALID_SOCKET) {
        fprintf(stderr, "ERROR: accept failed with %d\n", WSAGetLastError());
        free(client);
        return;
    }

    setsockopt(
        client->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&server->ms_timeout, sizeof(DWORD)
    );
    _beginthread(_handle_client, 2000, (void *)client);
}

void start_server(int32_t port, uint32_t ms_timeout) {
    _start_wsa();
    struct addrinfo *addr = _get_addr(port);
    server_t server = {
        .socket = _get_socket(addr),
        .ms_timeout = ms_timeout
    };

    _bind_socket(server.socket, addr);
    _listen_on_socket(server.socket);

    printf("LISTENING ON PORT %d\n", port);

    while (1) {
        _accept_connection(&server);
    }
}
