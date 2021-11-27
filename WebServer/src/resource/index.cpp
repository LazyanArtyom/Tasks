#include <iostream>
#include <string.h>

void get()
{
    std::cout << 
    "<h1>Main page</h1>\n\n <a href=\"/students\">Click to see student list</a>";
}

int main(int argc, char* argv[])
{
    if (argc > 1 && strcmp(argv[1], "GET") == 0)
        get();

    return 0;
}