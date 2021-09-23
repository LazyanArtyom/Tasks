#ifndef PROCMGR_PROCESSMANAGER_H_
#define PROCMGR_PROCESSMANAGER_H_

#include "process.h"
#include <queue>

namespace procmgr
{

class ProcessManager
{
    using ProcessType = proc::Process;
    using ProcessFifoContainerType = std::queue<ProcessType>;
public:
    void exec();
    void addProcess(ProcessType&& proc);

private:
    ProcessFifoContainerType processes_;    
};

} // namespace procmgr
#endif // PROCMGR_PROCESSMANAGER_H_