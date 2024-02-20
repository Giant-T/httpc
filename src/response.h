#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <WS2tcpip.h>
#include <stdint.h>

#include "request.h"

void respond(request_t* request, SOCKET client);
void respond_error(SOCKET client, uint16_t code);
size_t get_file_content(char* url, char** content);
void _handle_file_error(SOCKET client, char* url);

#endif  // RESPONSE_H_
