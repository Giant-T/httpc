#ifndef REQUEST_H_
#define REQUEST_H_

#include <stdint.h>
#include "header.h"

/***********************************************************************************
 * PUBLIC
 ***********************************************************************************/

typedef struct {
    char* path;
    Headers headers;
} Request;

/*!
 * @brief Parses the request contained inside a buffer sent by a client.
 * @param buf The buffer that needs to be parsed.
 * @returns A request struct that needs its headers freed using the function free_request.
 */
Request parse_request(char* buf);

/*!
 * @brief Initializes a request struct with dynamically allocated array of headers.
 * @param headers_size The starting size of the headers array.
 * @returns A newly constructed request with an empty array of headers.
 */
void request_init(Request* request, uint32_t headers_size);
/*!
 * @brief Frees the request's headers and resets its length and size to 0.
 * @param request The pointer to the request which headers needs freeing.
 */
void request_free(Request* request);

/***********************************************************************************
 * PRIVATE
 ***********************************************************************************/

/*!
 * @brief Parses the path part of the first line of the request.
 * @param line The first line of the request.
 * @returns The path as a string.
 */
char* _parse_path(char* line);

#endif  // REQUEST_H_
