#include "server.h"

#include <WS2tcpip.h>
#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>

#include "print.h"
#include "request.h"
#include "response.h"

#define REQUEST_CHUNK 512

void _start_wsa(void) {
    uint16_t version_requested = MAKEWORD(2, 2);
    WSADATA wsa_data;
    int32_t err = WSAStartup(version_requested, &wsa_data);

    if (err != 0) {
        print_err("WSAStartup failed with error %d.\n", err);
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
        print_err("getaddrinfo failed with error %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return res;
}

SOCKET _get_socket(struct addrinfo *addr) {
    SOCKET sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sfd == INVALID_SOCKET) {
        print_err("socket failed with error %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return sfd;
}

void _bind_socket(SOCKET sfd, struct addrinfo *addr) {
    int32_t err = bind(sfd, addr->ai_addr, addr->ai_addrlen);

    if (err != 0) {
        print_err("bind failed with error %d\n", WSAGetLastError());
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void _listen_on_socket(SOCKET sfd) {
    if (listen(sfd, SOMAXCONN) == SOCKET_ERROR) {
        print_err("listen failed with error %d\n", WSAGetLastError());
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

    char *rq_buf = malloc(REQUEST_CHUNK);
    size_t rq_buf_size = REQUEST_CHUNK;
    size_t rq_buf_len = 0;

    memset(rq_buf, 0, rq_buf_size);

    _print_addr(&client->addr);

    int n = 0;
    while (true) {
        n = recv(client->socket, rq_buf + n, REQUEST_CHUNK, 0);
        if (n > 0) {
            rq_buf_len += n;
            if (rq_buf[rq_buf_len - 1] == '\n') {
                request_t request = parse_request(rq_buf);
                respond(&request, client->socket);
                free_request(&request);
                break;
            } else if (rq_buf_size == rq_buf_len) {
                rq_buf_size += REQUEST_CHUNK;
                rq_buf = realloc(rq_buf, rq_buf_size);
            }
        } else {
            if (n == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err == WSAETIMEDOUT) {
                    printf("REQUEST: TIMED OUT\n");
                    char response[] = "HTTP/1.1 408 Request Timeout\nConnection: close\n\n";
                    send(client->socket, response, strlen(response), 0);
                } else {
                    print_err("recv failed with %d\n", err);
                }
            }

            break;
        }
    }

    closesocket(client->socket);
    free(rq_buf);
    free(client);
}

void _accept_connection(server_t *server) {
    client_t *client = malloc(sizeof(client_t));
    int32_t addr_len = sizeof(client->addr);
    client->socket = accept(server->socket, (struct sockaddr *)&client->addr, &addr_len);

    if (client->socket == INVALID_SOCKET) {
        print_err("accept failed with %d\n", WSAGetLastError());
        free(client);
        return;
    }

    int err = setsockopt(
        client->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&server->ms_timeout, sizeof(DWORD)
    );

    if (err != 0) {
        print_err("setsockopt failed with %d\n", WSAGetLastError());
        free(client);
        return;
    }

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

    while (true) {
        _accept_connection(&server);
    }
}
