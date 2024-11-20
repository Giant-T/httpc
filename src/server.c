#include "server.h"

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "print.h"
#include "request.h"
#include "response.h"

#define REQUEST_CHUNK 512

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
        print_err("getaddrinfo failed with error %s\n", gai_strerror(err));
        exit(1);
    }

    return res;
}

SOCKET _get_socket(struct addrinfo *addr) {
    SOCKET sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sfd == -1) {
        print_err("socket failed with error %d\n", errno);
        exit(1);
    }

    return sfd;
}

void _bind_socket(SOCKET sfd, struct addrinfo *addr) {
    int32_t err = bind(sfd, addr->ai_addr, addr->ai_addrlen);

    if (err == -1) {
        print_err("bind failed with error %d\n", errno);
        close(sfd);
        exit(EXIT_FAILURE);
    }
}

void _listen_on_socket(SOCKET sfd) {
    if (listen(sfd, SOMAXCONN) == -1) {
        print_err("listen failed with error %d\n", errno);
        close(sfd);
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

void *_handle_client(void *arg) {
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
            if (n == -1) {
                int err = errno;
                if (err == ETIMEDOUT) {
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

    close(client->socket);
    free(rq_buf);
    free(client);
    return NULL;
}

void _accept_connection(server_t *server) {
    client_t *client = malloc(sizeof(client_t));
    socklen_t addr_len = sizeof(client->addr);
    client->socket = accept(server->socket, (struct sockaddr *)&client->addr, &addr_len);

    if (client->socket == -1) {
        print_err("accept failed with %d\n", errno);
        free(client);
        return;
    }

    int err = setsockopt(
        client->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&server->timeout, sizeof(struct timeval)
    );

    if (err == -1) {
        print_err("setsockopt failed with %d\n", errno);
        free(client);
        return;
    }

    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&thread, &attr, _handle_client, (void *)client);
}

void start_server(int32_t port, struct timeval* timeout) {
    struct addrinfo *addr = _get_addr(port);
    server_t server = {
        .socket = _get_socket(addr),
    };
    memcpy(&server.timeout, timeout, sizeof(struct timeval));

    _bind_socket(server.socket, addr);
    _listen_on_socket(server.socket);

    printf("LISTENING ON PORT %d\n", port);

    while (true) {
        _accept_connection(&server);
    }
}
