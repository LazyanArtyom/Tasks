#include "threadpool.h"

namespace concurrency
{

thread_local threadsafe::WorkStealingQueue *ThreadPool::localTaskQueue_ = nullptr;
thread_local unsigned ThreadPool::index_ = -1;

ThreadPool::ThreadPool() : done_(false)
{
    const unsigned threadCount = std::thread::hardware_concurrency();
    try
    {
        for (size_t i = 0; i < threadCount; ++i)
        {
            localTaskQueues_.push_back(
                std::unique_ptr<threadsafe::WorkStealingQueue>(new threadsafe::WorkStealingQueue));
        }

        for (size_t i = 0; i < threadCount; ++i)
        {
            threads_.push_back(
                std::thread(&ThreadPool::workerThread, this, i));       
        }
    }
    catch(...)
    {
        done_ = true;
        throw;
    }
}

ThreadPool::~ThreadPool() 
{ 
    done_ = true; 

    for (size_t i = 0; i < threads_.size(); ++i)
        if (threads_[i].joinable())
            threads_[i].join();
}

void ThreadPool::runPendingTask()
{
    TaskType task;
    if (popTaskFromLocalQueue(task) ||
        popTaskFromPoolQueue(task)  ||
        popTaskFromOtherThreadQueue(task))
    {
        task();
    }
    else
    {
        std::this_thread::yield();
    }
}

void ThreadPool::workerThread(unsigned index)
{
    index_ = index;
    localTaskQueue_ = localTaskQueues_[index].get();

    while (!done_)
    {
        runPendingTask();
    }
}

bool ThreadPool::popTaskFromLocalQueue(TaskType& task)
{
    return localTaskQueue_ && localTaskQueue_->tryPop(task);
}

bool ThreadPool::popTaskFromPoolQueue(TaskType& task)
{
    return poolTaskQueue_.tryPop(task);
}

bool ThreadPool::popTaskFromOtherThreadQueue(TaskType& task)
{
    for (size_t i = 0; i < localTaskQueues_.size(); ++i)
    {
        const auto index = (index_ + i + 1) % localTaskQueues_.size();
        if (localTaskQueues_[index]->trySteal(task))
        {
            return true;
        }
    }
    return false;
}

} // end concurrency