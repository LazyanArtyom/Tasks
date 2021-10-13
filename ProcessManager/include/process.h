#ifndef PROC_PROCESS_H_
#define PROC_PROCESS_H_

#include <vector>
#include <stdexcept>

namespace proc
{

class Fifo
{
  public:
    Fifo(const std::string& fifoPath);

    ~Fifo();
    Fifo(const Fifo&) = delete;
    Fifo(Fifo&& other);

    /**
     * Reads up to length bytes from the fifo, placing them in buf.
     */
    ssize_t read(char* buf, uint64_t length);

    /**
     * Writes length bytes from buf to the fifo.
     */
    void write(const char* buf, uint64_t length);

    void close();
    int open(int flag);

    /**
     * Redirects the given file descriptor to the given descriptor.
     */
    void dup(int fd);
    std::string getPath() const;

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
    int fd_;
    std::string fifoPath_;
};

class Process
{
  public:
    /**
     * Constructs a new child Process, executing the given application and
     * passing the given arguments to it.
     */
    template <class... Args>
    Process(std::string fifoPath, std::string application, Args&&... args)
        : fifo_{Fifo(fifoPath)},
          args_{std::move(application), std::forward<Args>(args)...}
    {
    }

    ~Process();
    Process(Process&&) = default;
    Process(const Process&) = delete;

    /**
     * Executes the Process.
     */
    void exec();
    void wait();

    bool  isRunning() const;
    bool  isExited()  const;
    bool  isKilled()  const;
    bool  isStopped() const;
    pid_t getId()     const;

    /**
     * Gets the exit code for the child. If it was killed or stopped, the
     * signal that did so is returned instead.
     */
    int getExitCode() const;

    /**
     * An exception type for any unrecoverable errors that occur during
     * Process operations.
     */
    class Exception : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

  private:
    pid_t pid_ = -1;
    Fifo  fifo_;

    int  status_;
    bool waited_ = false;
    std::vector<std::string> args_;
};

} // namespace proc
#endif // PROC_PROCESS_H_
