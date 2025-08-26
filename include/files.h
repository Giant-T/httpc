#ifndef FILES_H
#define FILES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char* extension;
    uint8_t* content;
    size_t size;
} File;

/**!
 * Should free file.content after use
 * @param file The pointer to the file struct in which we read the content
 * @param path The path of the file
 * @returns True if the file was correctly read, false otherwise
 */
bool read_file(File* file, const char* path);

/**!
 * The extension is valid for as long as the supplied string is
 * @param path The pointer to the path of the file
 * @returns The pointer to the string containing the extension
 */
const char *get_file_extension(const char *path);

#endif // FILES_H
