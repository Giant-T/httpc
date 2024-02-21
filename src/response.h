#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <WS2tcpip.h>
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
void respond(request_t* request, SOCKET client);
/*!
 * @brief Responds to the client with the specified HTTP code.
 * @param client Socket of the client
 * @param http_code The HTTP error code.
 */
void respond_error(SOCKET client, uint16_t http_code);

/***********************************************************************************
 * PRIVATE
 ***********************************************************************************/

/*!
 * @brief Gets the content of the file;
 * @param path Path of the file.
 * @param [out] content A pointer that must be freed to the output content of a file or NULL if an error occured.
 * @returns The size in bytes of the content of the file.
 */
size_t _get_file_content(char* path, char** content);
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
