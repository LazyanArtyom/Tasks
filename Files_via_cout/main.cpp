#include <fcntl.h>
#include <unistd.h>
#include <iostream>

// witouth using dup2() function
void redirect(const char* fname)
{
    int no = open(fname, O_RDWR|O_CREAT|O_APPEND, 0600);
    stdout->_fileno = no;
}

void redirect_safe(const char* fname)
{
    int out = open(fname, O_RDWR|O_CREAT|O_APPEND, 0600);
    if (!out)
    { 
        perror("error opening out.txt");
        exit(-1); 
    }
    
    // Reassigning a File Descriptor
    // dup2() to duplicate a file descriptor, 
    // which will allow us to redirect output
    // STDOUT_FILENO is a file desriptor of stdout 
    if (!dup2(out, STDOUT_FILENO))
    {
        perror("cannot redirect stdout"); 
        exit(-1); 
    }

    close(out);
}

int main()
{
    redirect("output.txt");
    std::cout << "Hello world\n" << std::endl;

    return 0;
}
