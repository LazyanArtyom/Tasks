#include "process.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fstream>
#include <iostream>

namespace proc
{

/////////////////////////////////////////
//
// Class Fifo
//
Fifo::Fifo(const std::string& fifoPath)
{
    fifoPath_ = fifoPath;
    remove(fifoPath_.c_str());

    if (mkfifo(fifoPath_.c_str(), 0777) == -1)
    {
        throw exception("mkfifo failed: "
                        + std::system_category().message(errno));
    }
}

Fifo::Fifo(Fifo&& other)
{
    fd_ = std::move(other.fd_);
    fifoPath_ = std::move(other.fifoPath_);
}

Fifo::~Fifo()
{
    close();
}

void Fifo::write(const char* buf, uint64_t length)
{
    auto bytes = ::write(fd_, buf, length);
    if (bytes == -1)
    {
        // interrupt, just attempt to write again
        if (errno == EINTR)
            return write(buf, length);
        // otherwise, unrecoverable error
        perror("pipe_t::write()");
        throw exception{"failed to write"};
    }
    if (bytes < static_cast<ssize_t>(length))
        write(buf + bytes, length - static_cast<uint64_t>(bytes));
}

ssize_t Fifo::read(char* buf, uint64_t length)
{
    auto bytes = ::read(fd_, buf, length);
    return bytes;
}

void Fifo::open(int flag)
{
    if (flag == O_WRONLY || flag == O_RDONLY)
        fd_ = ::open(fifoPath_.c_str(), flag);
    else
        throw exception{"Can't open fifo, bad flag"};

    if (fd_ == -1)
    {
        perror("open: ");
        throw exception{"Can't open file"};
    } 
}

void Fifo::close()
{
    if (fd_ != -1)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

void Fifo::dup(int fd)
{
    if (::dup2(fd_, fd) == -1)
    {
        perror("pipe_t::dup()");
        throw exception{"failed to dup"};
    }
}

std::string Fifo::getPath() const
{
    return fifoPath_;
}

/////////////////////////////////////////
//
// Class Process
//
bool running(pid_t pid);

Process::~Process()
{
    wait();
}

void Process::exec()
{
    if (pid_ != -1)
        throw Exception{"Process already started"};

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork()");
        throw Exception{"Failed to fork child Process"};
    }
    else if (pid == 0)
    {
        fifo_.open(O_WRONLY);
        fifo_.dup(STDOUT_FILENO);
        fifo_.close();

        std::vector<char*> args;
        args.reserve(args_.size() + 1);

        // reverse args
        for (auto& arg : args_)
            args.push_back(const_cast<char*>(arg.c_str()));
        
        args.push_back(nullptr);

        execvp(args[0], args.data());

        std::_Exit(EXIT_SUCCESS);
    }
    else
    {
        fifo_.open(O_RDONLY);
        
        /******************* Test Print to Console ***********************/
        const int BUFFER = 200;
        char buf[BUFFER];

        std::cout << "************* " << fifo_.getPath() << " *************************\n";
        while (fifo_.read(buf, BUFFER) > 0)
            std::cout << buf << std::endl;
        std::cout << "***********************************************************\n";
        /******************* Test Print to Console ***********************/

        pid_ = pid;
        fifo_.close();
    }
}

pid_t Process::getId() const
{
    return pid_;
}

void Process::wait()
{
    if (!waited_)
    {
        waitpid(pid_, &status_, 0);
        waited_ = true;
    }
}

bool Process::isRunning() const
{
    return ::proc::running(this->getId());
}

bool Process::isExited() const
{
    if (!waited_)
        throw Exception{"Process::wait() not yet called"};
    return WIFEXITED(status_);
}

bool Process::isKilled() const
{
    if (!waited_)
        throw Exception{"Process::wait() not yet called"};
    return WIFSIGNALED(status_);
}

bool Process::isStopped() const
{
    if (!waited_)
        throw Exception{"Process::wait() not yet called"};
    return WIFSTOPPED(status_);
}

int Process::getExitCode() const
{
    if (!waited_)
        throw Exception{"Process::wait() not yet called"};
    if (isExited())
        return WEXITSTATUS(status_);
    if (isKilled())
        return WTERMSIG(status_);
    if (isStopped())
        return WSTOPSIG(status_);
    return -1;
}

bool running(pid_t pid)
{
    bool result = false;
    if (pid != -1)
    {
        if (0 == ::kill(pid, 0))
        {
            int status;
            const auto r = ::waitpid(pid, &status, WNOHANG);
            if (-1 == r)
            {
                perror("waitpid()");
                throw Process::Exception{
                    "Failed to check Process state "
                    "by waitpid(): "
                    + std::system_category().message(errno)};
            }
            if (r == pid)
                // Process has changed its state. We must detect why.
                result = !WIFEXITED(status) && !WIFSIGNALED(status);
            else
                // No changes in the Process status. It means that
                // Process is running.
                result = true;
        }
    }

    return result;
}
/////////////////////////////////////////
} // namespace proc