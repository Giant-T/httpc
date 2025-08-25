#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t len;
    uint32_t size;
    char** headers;
} Headers;

/**!
 * Initialize a headers struct
 * @param headers A pointer to the headers struct that should be initialized
 * @param size The base size of the headers buffer
 */
void headers_init(Headers* headers, size_t size);

/**!
 * Adds a header to the headers buffer and resizes it if needed
 * @param headers A pointer to the headers struct
 * @param header The new header
 * TODO: change const char* for a Header struct
 */
void headers_add(Headers* headers, const char* header);

/**!
 * Frees the content of the headers struct
 * If you allocated the headers struct on the heap, you should free it yourself
 * @param headers A pointer to the header struct
 */
void headers_free(Headers* headers);

#endif // HEADER_H
