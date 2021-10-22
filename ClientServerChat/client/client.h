#ifndef MESSAGING_CLIENT_H_
#define MESSAGING_CLIENT_H_

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

class Client
{
public:
    Client(const std::string& ip, int port);
    
    void connect();

private:
    //static void strOverwriteStdout();
    //static void strTrimLf(char* arr, int len);

    static void* sendMessageHnadler(void*);
    static void* recieveMessageHandler(void*);

    static const int NAME_SIZE = 32;
    static const int BUFF_LENGTH = 2048;

    static int socketFd_;
    static char name_[NAME_SIZE];
    static volatile sig_atomic_t flag_ ;
    static struct sockaddr_in serverAddress_;
};

} // end messaging
#endif // MESSAGING_CLIENT_H_