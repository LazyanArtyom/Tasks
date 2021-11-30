#include <iostream>
#include "requestmanager.h"

struct GetRequest : public http::IFunctionRequest
{
    // students
    void operator()()
    {
        std::cout << 
        "<h1>Students page</h1>\n\n <a href=\"/students/1\"> Student 1</a> <a href=\"/students/1\"> Student 2</a>";
    }
    
    // students/{}
    void operator()(int number)
    {
        std::cout << 
        "<h1>This is Student" << number << "</h1>";
    }
};

int main(int argc, char* argv[])
{

    http::RequestManager mgr;
    
    mgr.registerRequest("GET", std::make_unique<GetRequest>());
    
    mgr.handleRequest(argc, argv);

    return 0;
}