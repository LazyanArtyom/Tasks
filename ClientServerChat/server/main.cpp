#include "server.h"

int main()
{
    messaging::Server server("127.0.0.1", 4444);
    server.start();

    return 0;
}