#ifndef TAF_TESTFRAMEWORK_H_
#define TAF_TESTFRAMEWORK_H_

#include "pipe.h"

#include <any>
#include <vector>
#include <memory>
#include <functional>

namespace taf {

class TestFramework
{
    using TestMapContainerType = std::vector<std::pair<std::string, std::function<bool()>>>;
public:
    TestFramework(bool isRedirectToFile) : isRedirectToFile_{isRedirectToFile} {}

    /**
    * Runs all tests and redirects to stdout or file
    */
    void runTests();
    void addTest(std::string testName, std::function<bool()> funcPtr);

    /**
    * An exception type for any unrecoverable errors that occur during
    * pipe operations.
    */
    class exception : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
private:
    bool isRedirectToFile_ = false;
    TestMapContainerType testcases_;
};

} // end namespace taf
#endif // TAF_TESTFRAMEWORK_H_