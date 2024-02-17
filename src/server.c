#include "server.h"

#include <WS2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>

typedef struct {
    SOCKET socket;
    struct sockaddr_storage addr;
} client_t;

static void _start_wsa(void) {
    WORD version_requested = MAKEWORD(2, 2);
    WSADATA wsa_data;
    int err = WSAStartup(version_requested, &wsa_data);

    if (err != 0) {
        fprintf(stderr, "ERROR: WSAStartup failed with error %d.\n", err);
        exit(EXIT_FAILURE);
    }
}

static struct addrinfo *_get_addr(int port) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char port_buf[10];
    sprintf(port_buf, "%d", port);

    int err = getaddrinfo(NULL, port_buf, &hints, &res);

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

static SOCKET _get_socket(struct addrinfo *addr) {
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

static void _bind_socket(SOCKET sfd, struct addrinfo *addr) {
    int err = bind(sfd, addr->ai_addr, addr->ai_addrlen);

    if (err != 0) {
        fprintf(
            stderr, "ERROR: bind failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

static void _listen_on_socket(SOCKET sfd) {
    if (listen(sfd, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(
            stderr, "ERROR: listen failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

static void _print_addr(struct sockaddr_storage *addr) {
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

static void _handle_client(void *arg) {
    client_t *client = (client_t *)arg;

    char buf[1024];
    int buflen = 1024;

    _print_addr(&client->addr);

    for (;;) {
        int n = recv(client->socket, buf, buflen, 0);

        if (n > 0 && buf[n - 1] == '\n') {
            break;
        } else if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                printf("REQUEST: TIMED OUT\n");
                char response[] = "HTTP/1.1 408 Request Timeout\nConnection: close";
                send(client->socket, response, strlen(response), 0);
            }

            closesocket(client->socket);
            free(client);
            return;
        }
    }

    char response[] = "HTTP/1.1 200 OK\ncontent-type: text/html\n\nTEST";
    send(client->socket, response, strlen(response), 0);
    closesocket(client->socket);

    free(client);
}

void start_server(int port, unsigned long ms_timeout) {
    _start_wsa();
    struct addrinfo *addr = _get_addr(port);

    SOCKET server = _get_socket(addr);

    _bind_socket(server, addr);
    _listen_on_socket(server);

    printf("LISTENING ON PORT %d\n", port);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(server, &fds);

    while (1) {
        if (select(0, &fds, NULL, NULL, NULL)) {
            client_t *client = malloc(sizeof(client_t));
            int addrlen = sizeof client->addr;

            client->socket = accept(server, (struct sockaddr *)&client->addr, &addrlen);

            setsockopt(
                client->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ms_timeout, sizeof(DWORD)
            );

            _beginthread(_handle_client, 2000, (void *)client);
        }
    }
}
