#include "server.h"

#include <iostream>

namespace messaging
{

int Server::userId_ = 0;
int Server::listenFd_ = 0;
pthread_t Server::threadId_;
unsigned int Server::clientCount_ = 0;
pthread_mutex_t Server::mutex_ = PTHREAD_MUTEX_INITIALIZER;
Server::clientType* Server::clients_[MAX_CLIENTS] = { nullptr };

Server::Server(const std::string &ip, int port)
{
    int option = 1;
    struct sockaddr_in serverAddress;

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddress.sin_port = htons(port);

    // Ignore pipe signals
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenFd_, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
    {
        perror("ERROR: setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(listenFd_, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("ERROR: Socket binding failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(listenFd_, 10) < 0)
    {
        perror("ERROR: Socket listening failed");
        exit(EXIT_FAILURE);
    }
}

void Server::start()
{
    int connFd = 0;

    std::cout << "=== WELCOME TO THE CHATROOM ===\n";

    while (true)
    {
        socklen_t clientLen = sizeof(clientAddress_);
        connFd = accept(listenFd_, (struct sockaddr *)&clientAddress_, &clientLen);

        // Check if max clients is reached
        if ((clientCount_ + 1) == MAX_CLIENTS)
        {
            std::cout << "Max clients reached. Rejected: ";
            close(connFd);

            continue;
        }

        // Client settings
        clientType *client = (clientType *)malloc(sizeof(clientType));
        client->address = clientAddress_;
        client->socketFd = connFd;
        client->userId = userId_++;

        // Add client to the queue and fork thread
        addClient(client);

        bzero(&threadId_, sizeof(pthread_t));
        if (pthread_create(&threadId_, nullptr, &handleClient, (void *)client) < 0)
        {
            perror("ERROR: Creating thread filed.\n");
            exit(EXIT_FAILURE);
        }

        // Reduce CPU usage
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}

void Server::addClient(Server::clientType *client)
{
    pthread_mutex_lock(&mutex_);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients_[i] == nullptr)
        {
            clients_[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&mutex_);
}

void Server::removeClient(int userId)
{
    pthread_mutex_lock(&mutex_);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients_[i] != nullptr)
        {
            if (clients_[i]->userId == userId_)
            {
                clients_[i] = nullptr;
                break;
            }
        }
    }

    pthread_mutex_unlock(&mutex_);
}

void Server::sendMessage(char *msg, int userId)
{
    pthread_mutex_lock(&mutex_);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients_[i] != nullptr)
        {
            if (clients_[i]->userId != userId)
            {
                if (write(clients_[i]->socketFd, msg, strlen(msg)) < 0)
                {
                    perror("ERROR: write to descriptor failed");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&mutex_);
}

void *Server::handleClient(void *arg)
{
    char buffOut[BUFF_LENGTH] = {};
    char name[NAME_LENGTH] = {};
    int leaveFlag = 0;

    ++clientCount_;
    clientType *client = (clientType *)arg;

    // Name
    if (recv(client->socketFd, name, NAME_LENGTH, 0) <= 0 || strlen(name) < 2 || strlen(name) >= NAME_LENGTH - 1)
    {
        std::cout << "Didn't enter the name.\n";
        leaveFlag = 1;
    }
    else
    {
        strcpy(client->name, name);
        std::cout << client->name << " has joined.\n";
        sendMessage(buffOut, client->userId);
    }
	
    bzero(buffOut, BUFF_LENGTH);

    while (true)
    {
        if (leaveFlag)
        {
            break;
        }

        int receive = recv(client->socketFd, buffOut, BUFF_LENGTH, 0);
        if (receive > 0)
        {
            if (strlen(buffOut) > 0)
            {
                sendMessage(buffOut, client->userId);
                std::cout << "> " << client->name << ": " << buffOut << std::endl;
            }
        }
        else if (receive == 0 || strcmp(buffOut, "exit") == 0)
        {
            std::cout << client->name << " has left\n";
            sendMessage(buffOut, client->userId);
            leaveFlag = 1;
        }
        else
        {
            std::cout << "ERROR: -1\n";
            leaveFlag = 1;
        }

        bzero(buffOut, BUFF_LENGTH);
    }

    // Delete client from queue and yield thread
    close(client->socketFd);
    removeClient(client->userId);
    free(client);
    clientCount_--;
    pthread_detach(pthread_self());

    return nullptr;
}

} // end messaging