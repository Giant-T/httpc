#include <bits/types/struct_timeval.h>
#include "server.h"

int main(void) {
    const struct timeval timeout = {
        .tv_sec = 5,
    };
    start_server(8080, &timeout);
}
