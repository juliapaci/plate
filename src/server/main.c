#include "server.h"

int main(void) {
    Server server = init_state(8000);
    init_server(&server);
    clean_state(&server);
    return 0;
}
