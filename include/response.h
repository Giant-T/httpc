#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <sys/socket.h>
#include "server.h"
#include <stdint.h>

#include "request.h"

/***********************************************************************************
 * PUBLIC
 ***********************************************************************************/

/*!
 * @brief Responds to the client with the content of the file specified by the path.
 * @param request Pointer to the request of the client
 * @param client Socket of the client
 */
void respond(Request* request, SOCKET client);
/*!
 * @brief Responds to the client with the specified HTTP code.
 * @param client Socket of the client
 * @param http_code The HTTP error code.
 */
void respond_error(SOCKET client, uint16_t http_code);

/*!
 * @brief Gets the file extension part of the path.
 * @param path The file path
 * @returns The pointer to the start of the extension in the string
 */
char* _get_file_extension(char* path);
/*!
 * @brief Handles the file error and send the corresponding response to the client.
 * @param client Socket of the client
 * @param path The path of the file that caused an error
 */
void _handle_file_error(SOCKET client, char* path);

#endif  // RESPONSE_H_
