#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "request.h"
#include "response.h"

#include <memory>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace http
{
    
class Server
{
    static const int BUFF_LENGTH = 2048;

    typedef struct
    {
        struct sockaddr_in address;
        int socketFd;
        int userId;

    } clientType;

public:
    Server(const std::string& resourceDirectoryPath, const std::string& ip, int port);

    void start();
    static std::string execute(std::string method, std::string path, std::string arg);

private:
    static void* handleRequest(void* arg);
    static void sendResponse(std::shared_ptr<Response> httpResponse, clientType* client);

    static void processGetRequest(std::shared_ptr<Response> httpResponse, clientType* client);
//    static void processPostRequest(const std::string& requestedResource);
 //   static void processHeadRequest(const std::string& requestedResource);
 //   static void processDeleteRequest(const std::string& requestedResource);
 //   static void processOptionsRequest(const std::string& requestedResource);

    int userId_ = 0;
    int listenFd_ = 0;

    sockaddr_in clientAddress_;
    static std::string resourceDirectoryPath_;
};

} // end messaging
#endif // HTTP_SERVER_H_