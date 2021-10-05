#include "threadpool.h"
#include "timer.h"

#include <chrono>
#include <numeric>

template<typename Iterator, typename T>
class AccumlateBlock
{
public:
    T operator() (Iterator first, Iterator last)
    {
        return std::accumulate(first, last, T());
    }
};

template<typename Iterator, typename T>
T parralelAccumlate(Iterator first, Iterator last, T init)
{
    const size_t length = std::distance(first, last);
    if (!length)
        return init;

    const size_t blockSize = 25;
    size_t numBlocks = (length + blockSize - 1) / blockSize;

    std::vector<std::future<T>> futures(numBlocks - 1);
    concurrency::ThreadPool pool;

    Iterator blockStart = first;
    for (size_t i = 0; i < (numBlocks - 1); ++i)
    {
        Iterator blockEnd = blockStart;
        std::advance(blockEnd, blockSize);

        futures[i] = pool.submit([=]{
            return AccumlateBlock<Iterator, T>() (blockStart, blockEnd);
        });

        blockStart = blockEnd;
    }

    T lastResult = AccumlateBlock<Iterator, T>() (blockStart, last);
    T result = init;

    for (size_t i = 0; i < (numBlocks - 1); ++i)
    {
        result += futures[i].get();
    }
    
    result += lastResult;
    return result;
}

int main()
{
    std::vector<unsigned long long> data;
    data.reserve(100000000);
    for (int i = 0; i < 100000000; ++i)
        data.push_back(i);
    
    {   
        std::cout << "******* single thread *********\n";
        mytimer::Timer timer;
        int result = AccumlateBlock<std::vector<unsigned long long>::iterator, 
                                                unsigned long long>() (data.begin(), data.end());
        std::cout << result << std::endl;
        timer.stop();
        std::cout << "*******************************\n";
    }

    {
        std::cout << "******* concurrency threadcount: " << std::thread::hardware_concurrency() << " ********* \n";
        mytimer::Timer timer;
        int result = parralelAccumlate(data.begin(), data.end(), 0);
        std::cout << result << std::endl;
        timer.stop();
        std::cout << "*******************************\n";
    }

    return 0;
}