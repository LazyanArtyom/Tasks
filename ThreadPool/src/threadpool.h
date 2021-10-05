#ifndef CONCURRENCY_THREADPOOL_H_
#define CONCURRENCY_THREADPOOL_H_

#include "containers.h"

#include <atomic>
#include <thread>
#include <future>

namespace concurrency
{

/**
 * A thread pool that uses work stealing
 * This means that the thread pool can access the queue when trying to
 * steal a task for a thread that has no work to do.
 */
class ThreadPool
{
    using TaskType = threadsafe::FunctionWrapper;
    using LocalTaskQueueType = std::queue<TaskType>;
    using PoolTaskQueueType = threadsafe::Queue<TaskType>;
    
    using ThreadCollectionType = std::vector<std::thread>;
    using LocalTaskQueueCollectionType = std::vector<std::unique_ptr<threadsafe::WorkStealingQueue>>;

public:
    ThreadPool();
    ~ThreadPool();

    /**
     * Checks to see if the current thread has a work queue e. If it does,
     * it’s a pool thread, and puts the task on the local queue; otherwise,
     * puts the task on the pool queue.
     */
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType func)
    {
        using ResultType = typename std::result_of<FunctionType()>::type;
        std::packaged_task<ResultType()> task(func);

        std::future<ResultType> result(task.get_future());
        
        if (localTaskQueue_)
            localTaskQueue_->push(std::move(task));
        else
            poolTaskQueue_.push(std::move(task));
    
        return result;
    }

    /**
     * Tries to take a task off the queue and run it if there is one;
     * otherwise, it yields to allow the OS to reschedule the thread.
     */
    void runPendingTask();

private:
    void workerThread(unsigned index);
    bool popTaskFromLocalQueue(TaskType& task);
    bool popTaskFromPoolQueue(TaskType& task);
    
    /**
     * Iterates through the queues belonging
     * to all the threads in the pool, trying to steal a task from each in turn
     */
    bool popTaskFromOtherThreadQueue(TaskType& task);

private:
    std::atomic<bool> done_;
    ThreadCollectionType threads_;
    PoolTaskQueueType poolTaskQueue_;
    LocalTaskQueueCollectionType localTaskQueues_;

    // Variable to ensure that
    // each thread has its own task queue, as well as the global one.
    static thread_local threadsafe::WorkStealingQueue *localTaskQueue_;
    static thread_local unsigned index_;
};

} // end concurrency
#endif // CONCURRENCY_THREADPOOL_H_