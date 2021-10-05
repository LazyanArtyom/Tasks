#ifndef THREADSAFE_CONTAINERS_H_
#define THREADSAFE_CONTAINERS_H_

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace threadsafe {

template<typename T>
class Queue
{
public:
    Queue() = default;
    Queue(const Queue& other);

    void push(T value);

    void waitAndPop(T& value);
    std::shared_ptr<T> waitAndPop();
    
    bool tryPop(T& value);
    std::shared_ptr<T> tryPop();

    bool empty() const;

private:
    mutable std::mutex mutex_;
    std::queue<T> dataQueue_;
    std::condition_variable dataCond_;
};

template<typename T>
Queue<T>::Queue(const Queue& other)
{
    std::lock_guard<std::mutex> lock(other.mutex_);
    dataQueue_ = other.dataQueue_;
}

template<typename T>
void Queue<T>::push(T value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    dataQueue_.push(std::move(value));
    dataCond_.notify_one();
}

template<typename T>
void Queue<T>::waitAndPop(T& value)
{
    std::unique_lock<std::mutex> lock(mutex_);
    dataCond_.wait(lock, [this] { return !dataQueue_.empty(); });

    value = std::move(dataQueue_.front());
    dataQueue_.pop();
}

template<typename T>
std::shared_ptr<T> Queue<T>::waitAndPop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    dataCond_.wait(lock, [this] { return !dataQueue_.empty(); });

    std::shared_ptr<T> result(std::make_shared<T>(dataQueue_.front()));
    dataQueue_.pop();

    return result;
}

template<typename T>
bool Queue<T>::tryPop(T& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dataQueue_.empty())
        return false;

    value = std::move(dataQueue_.front());
    dataQueue_.pop();

    return true;
}

template<typename T>
std::shared_ptr<T> Queue<T>::tryPop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dataQueue_.empty())
        return std::shared_ptr<T>();
    
    std::shared_ptr<T> result(std::make_shared<T>(dataQueue_.front()));
    dataQueue_.pop();

    return result;
}

template<typename T>
bool Queue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return dataQueue_.empty();
}

/**
 * Custom function wrapper that can handle move-only types. This is a simple
 * type-erasure class with a function call operator.
 */
class FunctionWrapper
{
public:
    
    FunctionWrapper() = default;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;

    template<typename FuncType>
    FunctionWrapper(FuncType&& func) : impl_(new ImplType<FuncType>(std::move(func))) {}

    void operator()() { impl_->call(); }

    FunctionWrapper(FunctionWrapper&& other) : impl_(std::move(other.impl_)) {}
    FunctionWrapper& operator=(FunctionWrapper&& other)
    {
        impl_ = std::move(other.impl_);
        return *this;
    }

private:
    struct ImplBase
    {
        virtual void call() = 0;
        virtual ~ImplBase() {}
    };

    template<typename FuncType>
    struct ImplType : ImplBase
    {
        FuncType func_;
        ImplType(FuncType&& func) : func_(std::move(func)) {}
        void call() { func_(); }
    };

    std::unique_ptr<ImplBase> impl_;
};

/**
 * Lock-based queue for work stealing
 */
class WorkStealingQueue
{
    using DataType = FunctionWrapper;
    using DataQueueType = std::deque<DataType>;
public:
    WorkStealingQueue() = default;
    WorkStealingQueue(const WorkStealingQueue&) = delete;
    WorkStealingQueue& operator=(const WorkStealingQueue&) = delete;

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    void push(DataType data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_front(std::move(data));
    }

    bool tryPop(DataType& result)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
            return false;

        result = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    bool trySteal(DataType& result)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        return false;

        result = std::move(queue_.back());
        queue_.pop_back();
        return true;   
    }

private:
    DataQueueType queue_;
    mutable std::mutex mutex_;
};

} // end threadsafe
#endif // THREADSAFE_CONTAINERS_H_