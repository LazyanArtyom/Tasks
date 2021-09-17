#ifndef TAF_SINK_SINK_H_
#define TAF_SINK_SINK_H_

#include <sstream>

namespace taf {
namespace impl {

class ISink
{
public:
    virtual void log(const std::stringstream& str) = 0;     
};

} // end namespace impl
} // end namespace taf

#endif // TAF_SINK_SINK_H_ 