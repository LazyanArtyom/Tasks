#include "response.h"

#include <ctime>
#include <chrono>
#include <unordered_map>

namespace http
{
    std::unordered_map<unsigned int, std::string> StatusTable =
        {
            {101, "Switching Protocols"},
            {201, "Created"},
            {202, "Accepted"},
            {200, "200 OK"},
            {400, "Bad Request"},
            {401, "Unauthorized"},
            {404, "404 Not Found"},
            {408, "Request Timeout"},
            {413, "413 Request Entity Too Large"},
            {500, "500 Internal Server Error"},
            {501, "501 Not Implemented"},
            {502, "Bad Gateway"},
            {503, "Service Unavailable"},
            {505, "505 HTTP Version Not Supported"}};

    void Response::setMethod(HttpMethods method)
    {
        method_ = method;
    }

    HttpMethods Response::getMethod() const
    {
        return method_;
    }

    void Response::setResponseStatusCode(unsigned int status)
    {
        responseStatusCode_ = status;
    }

    std::unique_ptr<char[]> &Response::getResourceBuffer()
    {
        return resourceBuffer_;
    }


    std::string Response::getResource() const
    {
        return resource_;
    }

    void Response::setResource(const std::string &resource)
    {
        resource_ = resource;
    }

    void Response::setServerOption(const std::string &option)
    {
        serverOptions_ = option;
    }

    std::string Response::getServerOption() const
    {
        return serverOptions_;
    }

    void Response::setContentType(const std::string &contentType)
    {
        contentType_ = contentType_;
    }

    std::string Response::getContentType() const
    {
        return contentType_;
    }

    void Response::setResourceSizeInBytes(unsigned int size)
    {
        resourceSizeInBytes_ = size;
    }

    unsigned int Response::getResourceSizeInBytes() const
    {
        return resourceSizeInBytes_;
    }

    std::string Response::toString()
    {
        std::string responseStatus;

        auto end = std::chrono::system_clock::now();
        std::time_t endTime = std::chrono::system_clock::to_time_t(end);
        std::string timeStr(std::ctime(&endTime));

        auto statusLine = StatusTable[responseStatusCode_];

        responseStatus = std::string("HTTP/1.1 ") + statusLine + "\n";
        if (resourceSizeInBytes_ > 0)
        {
            responseStatus += std::string("Content-Length: ") +
                              std::to_string(resourceSizeInBytes_) + "\n";
        }
        if (!contentType_.empty())
        {
            responseStatus += contentType_ + "\n";
        }
        else
        {
            responseStatus += std::string("Content-Type: text/html") + "\n";
        }
        if (!serverOptions_.empty())
        {
            responseStatus += std::string("Allow: ") + std::move(serverOptions_) + "\n";
        }
        responseStatus += std::string("Server: HttpWebServer/0.0.1") + "\n";
        responseStatus += std::string("AcceptRanges: bytes") + "\n";
        responseStatus += std::string("Connection: Closed") + "\n";
        responseStatus += std::string("Date: ") + timeStr + "\n";

        if (resourceSizeInBytes_ > 0)
            responseStatus += resourceBuffer_.get();

        return responseStatus;
    }

} // namespace http
