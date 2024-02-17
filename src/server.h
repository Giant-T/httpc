#ifndef SERVER_H_
#define SERVER_H_

#include <WS2tcpip.h>

#include "winsock.h"

/***********************************************************************************
 * PUBLIC
 ***********************************************************************************/

/*!
 * @brief Starts a http server on the specified address and port.
 * @param port Port number (80 for http)
 * @param timeout Times out the connection after waiting for request.
 */
void start_server(int port, unsigned long ms_timeout);

/***********************************************************************************
 * PRIVATE
 ***********************************************************************************/

/*!
 * @brief Initiates the use of the Winsock DLL by the process
 */
static void _start_wsa(void);

/*!
 * @brief Gets the computer address informations.
 * @param port The port number
 */
static struct addrinfo *_get_addr(int port);
/*!
 * @brief Prints the address as a string depending on the address type.
 * @param addr The ipv4 address to be printed
 */
static void _print_addr(struct sockaddr_storage *addr);

/*!
 * @brief Gets a socket file descriptor associated with the address info.
 * @param addr Pointer to the address info struct.
 */
static SOCKET _get_socket(struct addrinfo *addr);
/*!
 * @brief Binds the socket to the address
 * @param sfd The socket file descriptor
 * @param addr The address that the socket will bind to
 */
static void _bind_socket(SOCKET sfd, struct addrinfo *addr);
/*!
 * @brief Marks the socket as a socket that can be connected to.
 * @param sfd The socket file descriptor
 */
static void _listen_on_socket(SOCKET sfd);

/*!
 * @brief Handles a connection with a client (receiving request and sending a response)
 * @param arg A void pointer to a client_t struct that must be freed.
 */
static void _handle_client(void *arg);

#endif  // SERVER_H_
