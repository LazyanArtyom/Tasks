#ifndef TAF_TESTFRAMEWORK_H_
#define TAF_TESTFRAMEWORK_H_

#include "sink/sink.h"

#include <any>
#include <vector>
#include <memory>
#include <functional>

namespace taf {

class TestFramework
{
    using SinkCollectionType = std::vector<std::unique_ptr<impl::ISink>>;
    using TestMapContainerType = std::vector<std::pair<std::string, std::function<bool()>>>;
public:
    void runTests();

    void log(const std::stringstream& str);
    void addSink(std::unique_ptr<impl::ISink> sinkPtr);
    void addTest(std::string testName, std::function<bool()> funcPtr);

private:
    SinkCollectionType   sinks_;
    TestMapContainerType testcases_;
};

} // end namespace taf
#endif // TAF_TESTFRAMEWORK_H_