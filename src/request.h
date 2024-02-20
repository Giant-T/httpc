#ifndef REQUEST_H_
#define REQUEST_H_

#include <stdint.h>

/***********************************************************************************
 * PUBLIC
 ***********************************************************************************/

typedef struct {
    char* url;
    uint32_t headers_len;
    uint32_t headers_size;
    char** headers;
} request_t;

/*!
 * @brief Parses the request contained inside a buffer sent by a client.
 * @param buf The buffer that needs to be parsed.
 * @returns A request struct that needs its headers freed using the function free_request.
 */
request_t parse_request(char* buf);
/*!
 * @brief Frees the request's headers and resets its length and size to 0.
 * @param request The pointer to the request which headers needs freeing.
 */
void free_request(request_t* request);

/***********************************************************************************
 * PRIVATE
 ***********************************************************************************/

/*!
 * @brief Parses the url part of the first line of the request.
 * @param line The first line of the request.
 * @returns The url as a string.
 */
char* _parse_url(char* line);

/*!
 * @brief Initializes a request struct with dynamically allocated array of headers.
 * @param headers_size The starting size of the headers array.
 * @returns A newly constructed request with an empty array of headers.
 */
request_t _initialize_request(uint32_t headers_size);
/*!
 * @brief Inserts a new header inside the request and reallocates if necessary.
 * @param The request that the header belongs to.
 * @param The inserted header.
 */
void _insert_header(request_t* request, const char* header);

#endif  // REQUEST_H_
