#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <map>
#include <string>
#include <memory>
#include <string.h>
#include <sstream>

namespace http
{
    struct IFunctionRequest
    {
        virtual void operator()() { return; }
        virtual void operator()(int) { return; }
        virtual void operator()(std::map<std::string, std::string>) { return; }

        virtual ~IFunctionRequest() {}
    };

    class RequestManager
    {
        using RequestFunctionPtr = std::unique_ptr<IFunctionRequest>;
        using RequestCollectionType = std::map<std::string, RequestFunctionPtr>;

    public:
        void handleRequest(int argc, char *argv[]);
        void registerRequest(std::string requestName, RequestFunctionPtr requestFunction);

    private:
        RequestCollectionType requestCollection_;
    };

    void RequestManager::handleRequest(int argc, char *argv[])
    {
        //std::cout << "Arg count: " << argc << std::endl;
        if (strcmp(argv[1], "GET") == 0)
        {
            if (strcmp(argv[2], "") == 0)
            {
                (*requestCollection_["GET"])();
            }
            else
            {
                int id = 0;
                std::stringstream buff(argv[2]);
                buff >> id;

                (*requestCollection_["GET"])(id);
            }
        }
    }

    void RequestManager::registerRequest(std::string requestName, RequestFunctionPtr requestFunction)
    {
        requestCollection_[requestName] = std::move(requestFunction);
    }

} // namespace http
#endif // HTTP_REQUEST_H_