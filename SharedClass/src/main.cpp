#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

class SharedClass
{
public:
	typedef void (*funcPtrType)(const char *);

	SharedClass()
	{
		std::cout << "Constructor\n";
	}

	void init()
	{
		// shared semaphore
		sem_init(&semaphore_, 1, 1);
	}

	void setMessage(const char *msg)
	{
		sem_wait(&semaphore_);
		msg_ = msg;

		if (counter_ == 0)
			callbacks_[0](msg);

		for (int i = 0; i < counter_; ++i)
			callbacks_[i](msg);
		
		sem_post(&semaphore_);
	}

	const char *getMessage()
	{
		return msg_;
	}

	void registerCallback(void (*f)(const char *msg))
	{
		sem_wait(&semaphore_);
		callbacks_[counter_] = f;
		++counter_;
		sem_post(&semaphore_);
	}

private:
	sem_t semaphore_;
	const char *msg_;
	int counter_ = 0;
	static const int MAXCALLBACKS = 30;
	funcPtrType callbacks_[MAXCALLBACKS];
};

int main()
{
	SharedClass *shared = (SharedClass* )mmap(nullptr, sizeof(SharedClass), PROT_READ | PROT_WRITE,
						   								MAP_SHARED | MAP_ANONYMOUS, -1, 0);

		
	if (shared == MAP_FAILED) 
	{
	    perror("mmap");
	    abort();
	}

	shared->init();

    int pid1 = fork();
    int pid2 = fork();
  
    if (pid1 > 0 && pid2 > 0) 
	{
		while (wait(nullptr) > 0);

		shared->registerCallback([](const char* msg){
			std::cout << "Parent recieved: ";
			std::cout << msg << std::endl;
		});		

		shared->setMessage("Parent created");

		munmap(shared, sizeof(SharedClass));
    }
    else if (pid1 == 0 && pid2 > 0)
    {
		shared->registerCallback([](const char* msg){
			std::cout << "First child recieved: ";
			std::cout << msg << std::endl;
		});		
	
		shared->setMessage("First child created");

		exit(EXIT_SUCCESS);
    }
    else if (pid1 > 0 && pid2 == 0)
    {
		shared->registerCallback([](const char* msg){
			std::cout << "Second child recieved: ";
			std::cout << msg << std::endl;
		});		
		
		shared->setMessage("Second child created");

		exit(EXIT_SUCCESS);
    }
    else 
	{
		shared->registerCallback([](const char* msg){
			std::cout << "Third child recieved: ";
			std::cout << msg << std::endl;
		});		
		
		shared->setMessage("Third child created");

		exit(EXIT_SUCCESS);
    }

	return 0;
}