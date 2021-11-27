#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <map>
#include <string>
#include <memory>

namespace http
{
    class Request
    {

    public:
        enum class HttpMethods
        {
            GET,
            HEAD,
            POST,
            PUT,
            DELETE,
            CONNECT,
            OPTIONS,
            TRACE,
        };

        void setStatus(unsigned int status);
        unsigned int getStatus() const;

        void setMethod(HttpMethods method);
        HttpMethods getMethod() const;

        void setRequest(const std::string &type);
        std::string getRequest();

        void setVersion(const std::string &version);
        std::string getVersion() const;

        void setResource(const std::string &resource);
        std::string getResource() const;

        void setContentType(const std::string &resource);
        std::string getContentType() const;

        std::string getPath() const;
        std::multimap<std::string, std::string> getQueryParameters() const;

    private:
        unsigned int status_;
        HttpMethods method_;
        std::string resource_;
        std::string version_;
        std::string request_;
    };

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    class RequestParser
    {
    public:
        RequestParser(std::string &);
        std::shared_ptr<Request> getRequest();

    private:
        std::string request_;
    };

} // namespace http
#endif // HTTP_REQUEST_H_