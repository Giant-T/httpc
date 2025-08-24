#ifndef FILES_H
#define FILES_H

#include <stddef.h>

typedef struct {
    char* content;
    size_t size;
} File;

/**!
 * Should free file.content after use
 * @returns True if the file was correctly read, false otherwise
 */
bool read_file(File* file, const char* path);

#endif // FILES_H
