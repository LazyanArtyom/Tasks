#include "testframework.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>

void taf::TestFramework::runTests()
{
    std::stringstream stream;

    for (const auto [testName, testFunc] : testcases_)
    {
        bool ret;
        pid_t pid = fork();

        if (pid < 0)
        {
            stream
                << "\n"
                << "In function " << testName << ": \n"
                << "Failed to create a child process!!"
                << std::endl;
            ret = false;
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                ret = (WEXITSTATUS(status) == EXIT_SUCCESS) ? true : false;
            }
            else
            {
                stream
                    << "\n"
                    << "In function " << testName << ": \n"
                    << "Failed to exit normally!!"
                    << std::endl;
                ret = false;
            }
        }
        else
        {
            bool isTrue = testFunc();
            exit(isTrue ? EXIT_SUCCESS : !EXIT_SUCCESS);
        }

        stream << std::left << std::setfill('-')
               << std::setw(50) << testName << " -->";

        if (ret)
        {
            stream << std::setw(10)
                   << std::left << "   OK "
                   << std::endl;
        }
        else
        {
            stream << std::setw(10)
                   << std::left << "   FAILED "
                   << std::endl;
        }
    }

    log(stream);
}

void taf::TestFramework::log(const std::stringstream &str)
{
    for (const auto &sink : sinks_)
        sink->log(str);
}

void taf::TestFramework::addSink(std::unique_ptr<impl::ISink> sinkPtr)
{
    sinks_.emplace_back(std::move(sinkPtr));
}

void taf::TestFramework::addTest(std::string testName, std::function<bool()> func)
{
    testcases_.emplace_back(testName, func);
}