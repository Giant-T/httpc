#include <WS2tcpip.h>
#include <process.h>
#include <windows.h>
#include <winsock.h>

#include "server.h"

#define WIN32_LEAN_AND_MEAN

int main(void) {
    start_server(80, 5000);
}
