#ifndef TIME_TIMER_H_
#define TIME_TIMER_H_

#include <chrono>
#include <thread>
#include <string>
#include <utility>
#include <algorithm>
#include <iostream>

namespace mytimer {

class Timer
{
    using TimePointType = std::chrono::high_resolution_clock::time_point;
public:
	~Timer();
	explicit Timer();
	Timer(const Timer& oRhs) = default;
	Timer(Timer&& oRhs) noexcept = default;
	Timer& operator=(const Timer& oRhs) = default;
	Timer& operator=(Timer&& oRhs) noexcept = default;

	void stop();
	
private:
	bool stopped_ = false;
	TimePointType startTimePoint_;
};

Timer::Timer() : startTimePoint_(std::chrono::high_resolution_clock::now()) {}

Timer::~Timer()
{
	if (!stopped_)
		stop();
}

void Timer::stop()
{
	const auto endPoint = std::chrono::high_resolution_clock::now();

	const long long end   = std::chrono::time_point_cast<std::chrono::seconds>(endPoint).time_since_epoch().count();
	const long long start = std::chrono::time_point_cast<std::chrono::seconds>(startTimePoint_).time_since_epoch().count();

	uint32_t const threadId = std::hash<std::thread::id> {} (std::this_thread::get_id());

    std::cout << "Thread Id: " << threadId << std::endl;
    std::cout << "Duration: " << end - start << " seconds." << std::endl;

	stopped_ = true;
}

} // end time
#endif // TIME_TIMER_H_