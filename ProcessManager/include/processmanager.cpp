#include "processmanager.h"

#include <iostream>

namespace procmgr
{

void ProcessManager::exec()
{
    while(!processes_.empty())
    {
        ProcessType proc = std::move(processes_.front());        
        proc.exec();
        proc.wait();
        std::cout << "ProcId: " << proc.getId() << " exit code: " << proc.getExitCode() << std::endl;
        processes_.pop();
    }
}

void ProcessManager::addProcess(ProcessType&& proc)
{
    processes_.emplace(std::forward<ProcessType>(proc));
}

} // namespace procmgr