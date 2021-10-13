#include "messagequeue.h"

#include <iostream>

using namespace std;

void firstHandler(const std::string& msg)
{
	std::cout << "firstHandler : " << msg << std::endl;
}

void secondHandler(const std::string& msg)
{
	std::cout << "secondHandler : " << msg << std::endl;
}

int main()
{	
	messaging::MessageQueue msgQueue;
	msgQueue.registerHandler(firstHandler);
	msgQueue.registerHandler(secondHandler);

	msgQueue.addMessage("aaaaaaaa");
	msgQueue.addMessage("bbbbbbbb");

	return 0;
}

