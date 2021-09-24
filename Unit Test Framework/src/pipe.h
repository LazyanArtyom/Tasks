#ifndef TAF_PIPE_H_
#define TAF_PIPE_H_

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <system_error>

namespace taf
{
/**
 * Represents a UNIX pipe between processes.
*/
class Pipe
{
public:
    static constexpr unsigned int READ_END = 0;
    static constexpr unsigned int WRITE_END = 1;

    /**
     * Wrapper type that ensures sanity when dealing with operations on
     * the different ends of the pipe.
     */
    class PipeEnd
    {
    public:
    /**
     * Constructs a new object to represent an end of a pipe. Ensures
     * the end passed makes sense (e.g., is either the READ_END or the
     * WRITE_END of the pipe).
     */
        PipeEnd(unsigned int end)
        {
            if (end != READ_END && end != WRITE_END)
                throw exception{"invalid pipe end"};
            end_ = end;
        }

        /**
         * pipe_ends are implicitly convertible to ints.
         */
        operator unsigned int() const
        {
            return end_;
        }

    private:
        unsigned int end_;
    };

    /**
     * Gets a pipe_end representing the read end of a pipe.
     */
    static PipeEnd read_end()
    {
        static PipeEnd read{READ_END};
        return read;
    }

    /**
    * Gets a pipe_end representing the write end of a pipe.
    */
    static PipeEnd write_end()
    {
        static PipeEnd write{WRITE_END};
        return write;
    }

    /**
    * Constructs a new pipe.
    */
    Pipe()
    {
        const auto r = ::pipe2(&pipe_[0], O_CLOEXEC);
        if (-1 == r)
            throw exception("pipe2 failed: " +
                            std::system_category().message(errno));
    }

    /**
    * Pipes may be move constructed.
    */
    Pipe(Pipe &&other)
    {
        pipe_ = std::move(other.pipe_);
        other.pipe_[READ_END] = -1;
        other.pipe_[WRITE_END] = -1;
    }

    /**
    * Pipes are unique---they cannot be copied.
    */
    Pipe(const Pipe&) = delete;

    /**
    * Writes length bytes from buf to the pipe.
    */
    void write(const char *buf, uint64_t length)
    {
        auto bytes = ::write(pipe_[WRITE_END], buf, length);
        if (bytes == -1)
        {
            // interrupt, just attempt to write again
            if (errno == EINTR)
                return write(buf, length);
            // otherwise, unrecoverable error
            perror("pipe::write()");
            throw exception{"failed to write"};
        }
        if (bytes < static_cast<ssize_t>(length))
            write(buf + bytes, length - static_cast<uint64_t>(bytes));
    }

    /**
    * Reads up to length bytes from the pipe, placing them in buf.
    */
    ssize_t read(char *buf, uint64_t length)
    {
        auto bytes = ::read(pipe_[READ_END], buf, length);
        return bytes;
    }

    /**
    * Closes both ends of the pipe.
    */
    void close()
    {
        close(read_end());
        close(write_end());
    }

    /**
    * Closes a specific end of the pipe.
    */
    void close(PipeEnd end)
    {
        if (pipe_[end] != -1)
        {
            ::close(pipe_[end]);
            pipe_[end] = -1;
        }
    }

    /**
    * Determines if an end of the pipe is still open.
    */
    bool open(PipeEnd end)
    {
        return pipe_[end] != -1;
    }

    /**
    * Redirects the given file descriptor to the given end of the pipe.
    *
    * @param end the end of the pipe to connect to the file descriptor
    * @param fd the file descriptor to connect
    */
    void dup(PipeEnd end, int fd)
    {
        if (::dup2(pipe_[end], fd) == -1)
        {
            perror("pipe::dup()");
            throw exception{"failed to dup"};
        }
    }

    /**
    * Redirects the given end of the given pipe to the current pipe.
    *
    * @param end the end of the pipe to redirect
    * @param other the pipe to redirect to the current pipe
    */
    void dup(PipeEnd end, Pipe &other)
    {
        dup(end, other.pipe_[end]);
    }

    /**
    * The destructor for pipes relinquishes any file descriptors that
    * have not yet been closed.
    */
    ~Pipe()
    {
        close();
    }

    /**
    * An exception type for any unrecoverable errors that occur during
    * pipe operations.
    */
    class exception : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

private:
    std::array<int, 2> pipe_;
};

} // namespace taf
#endif // TAF_PIPE_H_