#include <WS2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>

#define WIN32_LEAN_AND_MEAN

#define PORT 80
#define TIMEOUT 5000

typedef struct {
    SOCKET socket;
    struct sockaddr_storage addr;
} client_t;

void start_wsa(void) {
    WORD version_requested = MAKEWORD(2, 2);
    WSADATA wsa_data;
    int err = WSAStartup(version_requested, &wsa_data);

    if (err != 0) {
        fprintf(stderr, "ERROR: WSAStartup failed with error %d.\n", err);
        exit(EXIT_FAILURE);
    }
}

struct addrinfo *get_addr(void) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, "http", &hints, &res);

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

SOCKET get_socket(struct addrinfo *res) {
    SOCKET sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sfd == INVALID_SOCKET) {
        fprintf(
            stderr, "ERROR: socket failed with error %d\n", WSAGetLastError()
        );
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return sfd;
}

void bind_socket(SOCKET sfd, struct addrinfo *res) {
    int err = bind(sfd, res->ai_addr, res->ai_addrlen);

    if (err != 0) {
        fprintf(
            stderr, "ERROR: bind failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void listen_on_socket(SOCKET sfd) {
    if (listen(sfd, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(
            stderr, "ERROR: listen failed with error %d\n", WSAGetLastError()
        );
        closesocket(sfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void print_addr(struct sockaddr_storage *addr) {
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
            AF_INET, &((struct sockaddr_in6 *)addr)->sin6_addr, peer_name,
            INET_ADDRSTRLEN
        );
        printf("REQUEST: from %s\n", peer_name);
    }
}

void handle_client(void *arg) {
    client_t *client = (client_t *)arg;

    DWORD timeout = TIMEOUT;
    setsockopt(
        client->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(DWORD)
    );

    char buf[1024];
    int buflen = 1024;

    print_addr(&client->addr);

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

int main(void) {
    start_wsa();

    struct addrinfo *res = get_addr();

    SOCKET server = get_socket(res);

    bind_socket(server, res);
    listen_on_socket(server);

    printf("LISTENING ON PORT %d\n", PORT);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(server, &fds);

    while (1) {
        if (select(0, &fds, NULL, NULL, NULL)) {
            client_t *client = malloc(sizeof(client_t));
            int addrlen = sizeof client->addr;
            client->socket = accept(server, (struct sockaddr *)&client->addr, &addrlen);
            _beginthread(handle_client, 1000, (void *)client);
        }
    }
}
