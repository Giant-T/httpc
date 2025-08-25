#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char* key;
    char* value;
} Header;

typedef struct {
    uint32_t len;
    uint32_t size;
    Header* headers;
} Headers;

/**!
 * Splits a header into a key and value
 * You are responsible for freeing the strings
 * @param str The string representing the header
 */
char** split_header(char* line);

/**!
 * Initialize a header from a key and value pair
 * @param header A pointer to a header struct
 * @param key A pointer to a key string
 * @param value A pointer to a value string
 */
void header_init(Header* header, const char* key, const char* value);

/**!
 * Returns a string representation of the header
 * You are responsible for freeing the string
 * @param header A pointer to the header struct
 */
char* header_to_string(const Header* header);

/**!
 * Frees the resources used by the header
 * @param A pointer to the header struct
 */
void header_free(Header* header);

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
 */
void headers_add(Headers* headers, const char* key, const char* value);

/**!
 * Frees the content of the headers struct
 * If you allocated the headers struct on the heap, you should free it yourself
 * @param headers A pointer to the header struct
 */
void headers_free(Headers* headers);

#endif // HEADER_H
