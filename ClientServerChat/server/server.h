#ifndef MESSAGING_SERVER_H_
#define MESSAGING_SERVER_H_

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

namespace messaging
{

class Server
{
    static const int NAME_LENGTH = 32;
    static const int MAX_CLIENTS = 100;
    static const int BUFF_LENGTH = 2048;

    typedef struct
    {
        struct sockaddr_in address;
        int socketFd;
        int userId;
        char name[NAME_LENGTH];
    } clientType;

public:

    Server(const std::string &ip, int port);

    void start();

private:
    static void addClient(clientType* client);
    static void removeClient(int userId);

    static void* handleClient(void* arg);
    static void sendMessage(char* msg, int userId);

    static pthread_t threadId_;
    static pthread_mutex_t mutex_;
    struct sockaddr_in clientAddress_;
    static clientType* clients_[MAX_CLIENTS];

    static int userId_;
    static int listenFd_;
    static unsigned int clientCount_;
};

} // end messaging
#endif // MESSAGING_SERVER_H_