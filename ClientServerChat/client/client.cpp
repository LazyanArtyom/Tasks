#include "client.h"

#include <iostream>

namespace messaging
{

int Client::socketFd_ = 0;
char Client::name_[NAME_SIZE] = {};
volatile sig_atomic_t Client::flag_ = 0;
struct sockaddr_in Client::serverAddress_;

Client::Client(const std::string& ip, int port)
{
    // socket settings
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress_.sin_family = AF_INET;
    serverAddress_.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddress_.sin_port = htons(port);
}

void Client::connect()
{
    std::cout << "Please enter your name: ";
    fgets(name_, NAME_SIZE, stdin);

    // Connect to Server
    int err = ::connect(socketFd_, (struct sockaddr *)&serverAddress_, sizeof(serverAddress_));
    if (err == -1)
    {
        std::cout << "ERROR: connect\n";
        exit(EXIT_FAILURE);
    }
    // Send name
    send(socketFd_, name_, NAME_SIZE, 0);

    std::cout << "=== WELCOME TO THE CHATROOM ===\n";

    pthread_t sendMsgThread;
    if (pthread_create(&sendMsgThread, nullptr, sendMessageHnadler, this) < 0)
    {
        std::cout << "ERROR: pthread\n";
        exit(EXIT_FAILURE);
    }

    pthread_t recieveMsgThread;
    if (pthread_create(&recieveMsgThread, nullptr, recieveMessageHandler, this) < 0)
    {
        std::cout << "ERROR: pthread\n";
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        if (flag_)
        {
            std::cout << "\nBye\n";
            break;
        }
    }

    close(socketFd_);
}

void* Client::sendMessageHnadler(void*)
{
    char message[BUFF_LENGTH] = {};
    char buffer[BUFF_LENGTH + NAME_SIZE + 2] = {};

    while (true)
    {
        fgets(message, BUFF_LENGTH, stdin);
        if (strcmp(message, "exit") == 0)
        {
            break;
        }
        else
        {
            sprintf(buffer, "%s: %s\n", name_, message);
            send(socketFd_, buffer, strlen(buffer), 0);
        }

        bzero(message, BUFF_LENGTH);
        bzero(buffer, BUFF_LENGTH + NAME_SIZE);
    }

    flag_ = 1;
    return nullptr;
}

void* Client::recieveMessageHandler(void*)
{
    char message[BUFF_LENGTH] = {};
    while (true)
    {
        int receive = recv(socketFd_, message, BUFF_LENGTH, 0);
        if (receive > 0)
        {
            std::cout << "recived: " << message;
        }
        else if (receive == 0)
        {
            break;
        }
        else
        {
            // -1
        }

        memset(message, 0, sizeof(message));
    }
    return nullptr;
}

} // end messaging