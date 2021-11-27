#include "server.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <fcntl.h>
#include <sys/wait.h>
 
namespace http
{
    std::string Server::resourceDirectoryPath_ = "";
    
    // helpers
    bool isNumber(const std::string &str)
    {
        for (char const &c : str)
        {
            if (std::isdigit(c) == 0)
                return false;
        }
        return true;
    }

    Server::Server(const std::string& resourceDirectoryPath, const std::string &ip, int port)
    {
        resourceDirectoryPath_ = resourceDirectoryPath;

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

        std::cout << "=== Server is running... ===\n";

        while (true)
        {
            socklen_t clientLen = sizeof(clientAddress_);
            connFd = accept(listenFd_, (struct sockaddr *)&clientAddress_, &clientLen);

            clientType *client = reinterpret_cast<clientType *>(malloc(sizeof(clientType)));
            client->address = clientAddress_;
            client->socketFd = connFd;
            client->userId = userId_++;

            pthread_t threadId_;
            if (pthread_create(&threadId_, nullptr, &handleRequest, reinterpret_cast<void *>(client)) < 0)
            {
                perror("ERROR: Creating thread filed.\n");
                exit(EXIT_FAILURE);
            }

            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }

    std::string Server::execute(std::string method, std::string path, std::string arg)
    {
        pid_t pid;
        int nBytes;
        int pipeFd[2];

        std::string result;
        char buffer[1024];

        if (pipe(pipeFd) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        if ((pid = fork()) < 0)
        {
            perror("*** ERROR: forking child process failed\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            char *argv[] = {path.data(), method.data(), arg.data(), nullptr};

            close(pipeFd[0]);
            dup2(pipeFd[1], STDOUT_FILENO);

            if (execv(path.data(), argv) < 0)
            {
                perror("*** ERROR: exec failed\n");
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
        }

        close(pipeFd[1]);
        if ((nBytes = read(pipeFd[0], buffer, 1024)) > 0)
        {
            buffer[nBytes] = 0;
            result += buffer;
        }

        wait(nullptr);

        return result;
    }

    void Server::sendResponse(std::shared_ptr<Response> httpResponse, clientType *client)
    {
        if (client != nullptr)
        {
            std::string response = httpResponse->toString();

            std::cout << "[ Handling Client: " << client->userId << " ]" << std::endl;
            std::cout << "Response:\n{" << std::endl;
            std::cout << response;
            std::cout << "}" << std::endl;

            if (write(client->socketFd,
                      response.c_str(),
                      response.size()) < 0)
            {
                perror("ERROR: write to descriptor failed");
            }
        }
    }

    void Server::processGetRequest(std::shared_ptr<Response> response, clientType *client)
    {
        if (response->getResource().empty())
        {
            response->setResponseStatusCode(400);
            sendResponse(response, client);
            return;
        }

        if (response->getResource().compare("/") == 0)
        {
            response->setResource("/index");
        }

        std::string resource = response->getResource();
        std::size_t lastTokenLen = resource.find_last_of("/\\");
        std::string lastToken = resource.substr(lastTokenLen + 1);

        bool hasArg = false;
        std::string resourceFilePath = resourceDirectoryPath_ + resource;
        if (isNumber(lastToken))
        {
            hasArg = true;
            resourceFilePath = resource.substr(0, lastTokenLen) + ".out";
        }

        // execute program
        if (std::filesystem::exists(resourceFilePath + ".out"))
        {
            std::cout << "Found " << resourceFilePath + ".out file\n";

            std::string result = execute("GET", resourceFilePath + ".out", hasArg ? lastToken : std::string());

            std::cout << "Executet result = \n{\n"
                      << result << "\n}\n";

            response->setResourceSizeInBytes(static_cast<std::size_t>(result.size()));
            response->getResourceBuffer().reset(new char[response->getResourceSizeInBytes()]);

            strcpy(response->getResourceBuffer().get(), result.c_str());
            sendResponse(response, client);
            
            return;
        }

        if (!std::filesystem::exists(resourceFilePath))
        {
            response->setResponseStatusCode(404);
            sendResponse(response, client);
            return;
        }

        std::ifstream resourceFstream(resourceFilePath, std::ifstream::binary);

        if (!resourceFstream.is_open())
        {
            response->setResponseStatusCode(404);
            sendResponse(response, client);
            return;
        }

        resourceFstream.seekg(0, std::ifstream::end);
        response->setResourceSizeInBytes(static_cast<std::size_t>(resourceFstream.tellg()));

        response->getResourceBuffer().reset(new char[response->getResourceSizeInBytes()]);
        resourceFstream.seekg(std::ifstream::beg);
        resourceFstream.read(response->getResourceBuffer().get(), response->getResourceSizeInBytes());

        sendResponse(response, client);
    }
    /*
    void Server::processPostRequest(const std::string &requestedResource)
    {
    }

    void Server::processHeadRequest(const std::string &requestedResource)
    {
    }

    void Server::processDeleteRequest(const std::string &requestedResource)
    {
    }

    void Server::processOptionsRequest(const std::string &requestedResource)
    {
    }
*/

    void *Server::handleRequest(void *arg)
    {
        char buffOut[BUFF_LENGTH] = {};
        bzero(buffOut, BUFF_LENGTH);

        clientType *client = reinterpret_cast<clientType *>(arg);

        int receive = recv(client->socketFd, buffOut, BUFF_LENGTH, 0);
        if (receive > 0)
        {
            if (strlen(buffOut) > 0)
            {
                std::string request(buffOut);

                RequestParser parser(request);
                std::shared_ptr<Request> httpRequest = parser.getRequest();

                std::cout << "[ Handling Client: " << client->userId << " ]" << std::endl;
                std::cout << "Request:\n{" << std::endl;
                std::cout << httpRequest->getRequest();
                std::cout << "}" << std::endl;
                std::cout << "Resource: " << httpRequest->getResource() << std::endl;

                std::shared_ptr<Response> httpResponse = std::make_shared<Response>();
                httpResponse->setResource(httpRequest->getResource());
                httpResponse->setContentType(httpRequest->getContentType());

                switch (httpRequest->getMethod())
                {
                case Request::HttpMethods::GET:
                    processGetRequest(httpResponse, client);
                    break;
                // case HttpMethods::POST:
                //     processPostRequest(httpResponse);
                //     break;
                // case HttpMethods::HEAD:
                //     processHeadRequest(httpResponse);
                //     break;
                // case HttpMethods::DELETE:
                //     processDeleteRequest(httpResponse);
                //     break;
                // case HttpMethods::OPTIONS:
                //     processOptionsRequest(httpResponse);
                //     break;
                default:
                    break;
                }
            }
        }

        close(client->socketFd);
        free(client);
        pthread_detach(pthread_self());

        return nullptr;
    }

} // end messaging