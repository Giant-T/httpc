#ifndef SERVER_H_
#define SERVER_H_

#include <WS2tcpip.h>
#include <stdint.h>
#include <winsock2.h>

#include "winsock.h"

/***********************************************************************************
 * PUBLIC
 ***********************************************************************************/

typedef struct {
    SOCKET socket;
    struct sockaddr_storage addr;
} client_t;

typedef struct {
    SOCKET socket;
    uint32_t ms_timeout;
} server_t;

/*!
 * @brief Starts a http server on the specified address and port.
 * @param port Port number (80 for http)
 * @param timeout Times out the connection after waiting for request.
 */
void start_server(int32_t port, uint32_t ms_timeout);

/***********************************************************************************
 * PRIVATE
 ***********************************************************************************/

/*!
 * @brief Initiates the use of the Winsock DLL by the process
 */
void _start_wsa(void);
/*!
 * @brief Gets the computer address informations.
 * @param port The port number
 */
struct addrinfo *_get_addr(int32_t port);
/*!
 * @brief Prints the address as a string depending on the address type.
 * @param addr
 */
void _print_addr(struct sockaddr_storage *addr);
/*!
 * @brief Gets a socket file descriptor associated with the address info.
 * @param addr Pointer to the address info struct.
 */
SOCKET _get_socket(struct addrinfo *addr);
/*!
 * @brief Binds the socket to the address
 * @param sfd The socket file descriptor
 * @param addr The address that the socket will bind to
 */
void _bind_socket(SOCKET sfd, struct addrinfo *addr);
/*!
 * @brief Marks the socket as a socket that can be connected to.
 * @param sfd The socket file descriptor
 */
void _listen_on_socket(SOCKET sfd);
/*!
 * @brief Accepts a connection from a client and adds it to the server.
 * @param server The server that needs to accept the connection
 */
void _accept_connection(server_t *server);
/*!
 * @brief Handles a connection with a client (receiving request and sending a response)
 * @param arg Void pointer to a client that must be freed.
 */
void _handle_client(void *arg);

#endif  // SERVER_H_
