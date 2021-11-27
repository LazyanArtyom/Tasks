#include <iostream>
#include <string.h>

void get()
{
    std::cout << 
    "<h1>Students</h1> <p>Student1</p> <p>Student2</p>";
}

int main(int argc, char* argv[])
{
    if (argc > 1 && strcmp(argv[1], "GET") == 0)
        get();

    return 0;
}