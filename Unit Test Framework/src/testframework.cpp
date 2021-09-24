#include "testframework.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

void taf::TestFramework::runTests()
{   
    int fd = -1;
    if (isRedirectToFile_)
    {
        int fd = ::open("out.txt", O_RDWR|O_CREAT|O_APPEND, 0600);
        if (fd == -1)
        {
            perror("open file:");
            throw exception{"failed to open"};
        }
        ::dup2(fd, STDOUT_FILENO);
    }
    
    for (const auto [testName, testFunc] : testcases_)
    {
        Pipe pipe;
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cout
                << "\n"
                << "In function " << testName << ": \n"
                << "Failed to create a child process!!"
                << std::endl;
            throw exception{"Failed to create a child process!!"};
        }
        else if (pid > 0)
        {
            pipe.close(Pipe::write_end());

            bool ret;
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                char result[sizeof(bool)];
                auto bytes = pipe.read(result, sizeof(bool));
                if (bytes == sizeof(bool))
                    std::memcpy(&ret, result, sizeof(bool));
            }
            else
            {
                std::cout
                    << "\n"
                    << "In function " << testName << ": \n"
                    << "Failed to exit normally!!"
                    << std::endl;
                ret = false;
            }
            
            std::cout << std::left << std::setfill('-')
               << std::setw(50) << testName << " -->";

            if (ret)
            {
                std::cout << std::setw(10)
                    << std::left << "   OK "
                    << std::endl;
            }
            else
            {
                std::cout << std::setw(10)
                    << std::left << "   FAILED "
                    << std::endl;
            }
            pipe.close();
        }
        else
        {
            bool isTrue = testFunc();
            pipe.close(taf::Pipe::read_end());
            
            char result[sizeof(bool)];
            std::memcpy(result, &isTrue, sizeof(bool));
            pipe.write(result, sizeof(int));
            pipe.close();

            exit(isTrue ? EXIT_SUCCESS : !EXIT_SUCCESS);
        }
    }
    close(fd);
}

void taf::TestFramework::addTest(std::string testName, std::function<bool()> func)
{
    testcases_.emplace_back(testName, func);
}