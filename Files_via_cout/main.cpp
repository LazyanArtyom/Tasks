#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main()
{
    // As we know that the file descriptor returned by
    // a successful call will be the lowest-numbered file descriptor not
    // currently open for the process. :D
    // and 1 is a file descriptor of stdout
    close(1);
    int fd = open("output.txt", O_RDWR|O_CREAT|O_APPEND, 0600);
    std::cout << fd << "Hello world\n" << std::endl;

    return 0;
}
