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
    Header* headers; // TODO: replace by a hashtable
} Headers;

/**!
 * Splits a header into a key and value
 * You are responsible for freeing the strings
 * @param str The string representing the header
 */
char** split_header(char* line);

/**!
 * Initialize a header from a key and value pair
 * @param self A pointer to a header struct
 * @param key A pointer to a key string
 * @param value A pointer to a value string
 */
void header_init(Header* self, const char* key, const char* value);

/**!
 * Returns a string representation of the header
 * You are responsible for freeing the string
 * @param self A pointer to the header struct
 */
char* header_to_string(const Header* self);

/**!
 * Frees the resources used by the header
 * @param self A pointer to the header struct
 */
void header_free(Header* self);

/**!
 * Initialize a headers struct
 * @param self A pointer to the headers struct that should be initialized
 * @param size The base size of the headers buffer
 */
void headers_init(Headers* self, size_t size);

/**!
 * Adds a header to the headers buffer and resizes it if needed
 * @param self A pointer to the headers struct
 * @param header The new header
 */
void headers_add(Headers* self, const char* key, const char* value);

/**!
 * Returns a string representation of the headers.
 * You are responsible for freeing the string.
 * @param self A pointer to the headers struct
 */
char* headers_to_string(Headers* self);

/**!
 * Gets the header that matches the key or null if none match
 * @param self A pointer to the headers struct
 * @param key The string key
 * @returns A pointer to the header which matches the key
 */
Header* headers_get(Headers* self, const char* key);

/**!
 * Frees the content of the headers struct.
 * If you allocated the headers struct on the heap, you should free it yourself.
 * @param self A pointer to the header struct
 */
void headers_free(Headers* self);

#endif // HEADER_H
