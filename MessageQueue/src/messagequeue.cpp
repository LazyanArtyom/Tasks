#include "messagequeue.h"

#include <ctime>
#include <cstdlib>

#include <iostream>

#define MSG_EXIT_THREAD 1
#define MSG_POST_USER_DATA 2

namespace messaging
{

struct ThreadMsg
{
	ThreadMsg(int id, std::string msg) : id_(id), msg_(msg) {}

	int id_;
	std::string msg_;
};

WorkerThread::WorkerThread(MsgHandlerType handler)
	: handler_(handler), done_(false)
{
}

WorkerThread::~WorkerThread()
{
	exitThread();
}

bool WorkerThread::createThread()
{
	if (!thread_)
		thread_ = std::unique_ptr<std::thread>(new std::thread(&WorkerThread::process, this));
	return true;
}

std::thread::id WorkerThread::getThreadId()
{
	return thread_->get_id();
}

std::thread::id WorkerThread::getCurrentThreadId()
{
	return std::this_thread::get_id();
}

void WorkerThread::exitThread()
{
	if (!thread_)
		return;

	std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_EXIT_THREAD, std::string()));

	// Put exit thread message into the queue
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push(threadMsg);
		dataCond_.notify_one();
	}

	thread_->join();
	thread_ = nullptr;
}

void WorkerThread::postMsg(std::string msg)
{
	std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_POST_USER_DATA, msg));

	std::unique_lock<std::mutex> lock(mutex_);
	queue_.push(threadMsg);
	dataCond_.notify_one();
}

void WorkerThread::process()
{
	done_ = false;

	while (true)
	{
		std::shared_ptr<ThreadMsg> msg;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			while (queue_.empty())
				dataCond_.wait(lock);

			if (queue_.empty())
				continue;

			msg = queue_.front();
			queue_.pop();
		}

		switch (msg->id_)
		{
			case MSG_POST_USER_DATA:
			{
				handler_("ThreadId: " + std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()))
					     + " MsgRecived: " + msg->msg_);
				break;
			}

			case MSG_EXIT_THREAD:
			{
				done_ = true;
				return;
			}
		}
	}
}

MessageQueue::~MessageQueue()
{
	for (auto& thread : threads_)
		thread->exitThread();
}

void MessageQueue::addMessage(const std::string& msg)
{
	for (auto& thread : threads_)
		thread->postMsg(msg);
}

void MessageQueue::registerHandler(MsgHandlerType handler)
{
	srand((unsigned)time(NULL));
	int threadId = rand() % 10;

	threads_.emplace_back(
		std::unique_ptr<WorkerThread>(new WorkerThread(handler)));

	threads_.back()->createThread();	
}

} // end messaging
