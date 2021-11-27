#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <map>
#include <string>
#include <memory>

namespace http
{
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

    class Response
    {
    public:
        void setMethod(HttpMethods method);
        HttpMethods getMethod() const;

        void setResponseStatusCode(unsigned int status);

        std::unique_ptr<char[]> &getResourceBuffer();

        std::string getResource() const;
        void setResource(const std::string &resource);

        void setServerOption(const std::string &option);
        std::string getServerOption() const;

        void setContentType(const std::string &contentType);
        std::string getContentType() const;

        void setResourceSizeInBytes(unsigned int size);
        unsigned int getResourceSizeInBytes() const;

        std::string toString();

    private:
        HttpMethods method_;
        std::string resource_;
        std::string contentType_;
        std::string serverOptions_;
        unsigned int responseStatusCode_ = 0;
        unsigned int resourceSizeInBytes_ = 0;
        std::unique_ptr<char[]> resourceBuffer_;
    };

} // namespace http
#endif // HTTP_RESPONSE_H_