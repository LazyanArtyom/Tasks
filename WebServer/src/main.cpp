#include "server.h"
#include <iostream>

int main()
{
    http::Server server("/home/tyom/Desktop/LevelUPTasks/LevelUPTasks/WebServer/src/resource",
                        "127.0.0.1", 
                        4444);
    server.start();

    return 0;
}