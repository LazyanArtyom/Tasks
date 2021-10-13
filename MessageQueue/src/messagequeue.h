#ifndef MESSAGING_MESSAGEQUEUE_H_
#define MESSAGING_MESSAGEQUEUE_H_

#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>
#include <condition_variable>

namespace messaging
{

struct ThreadMsg;

class WorkerThread
{
    using ThreadPtrType = std::unique_ptr<std::thread>;
    using MsgHandlerType = std::function<void(std::string)>;
    using MsgQueueType = std::queue<std::shared_ptr<ThreadMsg>>;

public:
    WorkerThread(MsgHandlerType handler);

    ~WorkerThread();


    bool createThread();
    void exitThread();

    std::thread::id getThreadId();
    static std::thread::id getCurrentThreadId();

    void postMsg(std::string msg);

private:
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;

    void process();

    MsgHandlerType handler_;
    MsgQueueType queue_;
    ThreadPtrType thread_ = nullptr;

    std::mutex mutex_;
    std::condition_variable dataCond_;
    std::atomic<bool> done_;
};

class MessageQueue
{
    using MsgHandlerType = std::function<void(std::string)>;
    using WorkerThreadCollectionType = std::vector<std::unique_ptr<WorkerThread>>;
public:

    ~MessageQueue();
    void addMessage(const std::string& msg);
    void registerHandler(MsgHandlerType handler);

private:
    WorkerThreadCollectionType threads_;
};

} // end messaging
#endif // MESSAGING_MESSAGEQUEUE_H_