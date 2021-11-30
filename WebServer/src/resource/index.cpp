#include <iostream>
#include "requestmanager.h"

struct GetRequest : public http::IFunctionRequest
{
    void operator()()
    {
        std::cout << 
        "<h1>Main page</h1>\n\n <a href=\"/students\">Click to see student list</a>";
    }
};

int main(int argc, char* argv[])
{

    http::RequestManager mgr;
    
    mgr.registerRequest("GET", std::make_unique<GetRequest>());
    
    mgr.handleRequest(argc, argv);

    return 0;
}