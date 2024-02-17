#ifndef REQUEST_H_
#define REQUEST_H_

#include <stdint.h>

typedef struct {
    char* file;
    uint32_t headers_len;
    uint32_t headers_size;
    char** headers;
} request_t;

request_t parse_request(char* buf, uint32_t len);
void free_request(request_t* request);

request_t _initialize_request(uint32_t headers_size);
void _insert_header(request_t* request, char* header);
#endif  // REQUEST_H_
