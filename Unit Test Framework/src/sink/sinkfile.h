#ifndef TAF_SINK_SINKFILE_H_
#define TAF_SINK_SINKFILE_H_

#include "sink.h"

namespace taf {

class SinkFile : public impl::ISink
{
public:
    SinkFile(const std::string& fileName);
    void log(const std::stringstream& str) override;     

private:
    std::string fileName_;
};

} // end namespace taf
#endif // TAF_SINK_SINKFILE_H_