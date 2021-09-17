#ifndef TAF_SINK_SINKCONSOLE_H_
#define TAF_SINK_SINKCONSOLE_H_

#include "sink.h"

namespace taf {

class SinkConsole : public impl::ISink
{
public:
    void log(const std::stringstream& str) override;     

private:
};

} // end namespace TAF
#endif // TAF_SINK_SINKCONSOLE_H_