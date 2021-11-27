#include "request.h"

#include <sstream>
#include <unordered_map>

namespace http
{

    std::unordered_map<std::string, std::string> ConentTypeTable =
        {
            {"gif", "image/gif"},
            {"jpeg", "image/jpeg"},
            {"png", "image/png"},
            {"css", "text/css"},
            {"csv", "text/csv"},
            {"html", "text/html"}};

    void Request::setStatus(unsigned int status)
    {
        status_ = status;
    }

    unsigned int Request::getStatus() const
    {
        return status_;
    }

    void Request::setMethod(HttpMethods method)
    {
        method_ = method;
    }

    Request::HttpMethods Request::getMethod() const
    {
        return method_;
    }

    void Request::setRequest(const std::string &type)
    {
        request_ = type;
    }

    std::string Request::getRequest()
    {
        return request_;
    }

    void Request::setVersion(const std::string &version)
    {
        version_ = version;
    }

    std::string Request::getVersion() const
    {
        return version_;
    }

    void Request::setResource(const std::string &resource)
    {
        resource_ = resource;
    }

    std::string Request::getResource() const
    {
        return resource_;
    }

    std::string Request::getContentType() const
    {
        size_t pos = resource_.rfind('.');
        if (pos == std::string::npos)
            return std::string();

        std::string ext = resource_.substr(pos + 1);

        if (ext == "jpg")
            ext == "jpeg";

        return ConentTypeTable[ext];
    }

    /////////////////////////////////////////////////

    RequestParser::RequestParser(std::string &request) : request_(request) {}

    std::shared_ptr<Request> RequestParser::getRequest()
    {
        if (request_.empty())
            return nullptr;

        std::string requestMethod, resource, httpVersion;
        std::istringstream requestLineStream(request_);

        //extract request method, GET, POST, .....
        requestLineStream >> requestMethod;
        //extract requested resource
        requestLineStream >> resource;
        //extract HTTP version
        requestLineStream >> httpVersion;

        std::shared_ptr<Request> request = std::make_shared<Request>();

        request->setResource(std::move(resource));
        request->setStatus(0);

        if (requestMethod.compare("GET") == 0)
        {
            request->setMethod(Request::HttpMethods::GET);
        }
        else if (requestMethod.compare("HEAD") == 0)
        {
            request->setMethod(Request::HttpMethods::HEAD);
        }
        else if (requestMethod.compare("POST") == 0)
        {
            request->setMethod(Request::HttpMethods::POST);
        }
        else if (requestMethod.compare("PUT") == 0)
        {
            request->setMethod(Request::HttpMethods::PUT);
        }
        else if (requestMethod.compare("DELETE") == 0)
        {
            request->setMethod(Request::HttpMethods::DELETE);
        }
        else if (requestMethod.compare("CONNECT") == 0)
        {
            request->setMethod(Request::HttpMethods::CONNECT);
        }
        else if (requestMethod.compare("OPTIONS") == 0)
        {
            request->setMethod(Request::HttpMethods::OPTIONS);
        }
        else if (requestMethod.compare("TRACE") == 0)
        {
            request->setMethod(Request::HttpMethods::TRACE);
        }
        else
        {
            request->setStatus(400);
        }

        if (httpVersion.compare("HTTP/1.1") == 0)
        {
            request->setVersion("1.1");
        }
        else
        {
            request->setStatus(505);
        }

        request->setRequest(std::move(request_));

        return request;
    }

} // namespace http
