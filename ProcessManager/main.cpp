#include "include/process.h"
#include "include/processmanager.h"

int main()
{
    procmgr::ProcessManager procMgr;
    procMgr.addProcess(proc::Process{"/tmp/fifo1", "echo", "test1"});
    procMgr.addProcess(proc::Process{"/tmp/fifo2", "echo", "test2"});
    procMgr.exec();

    return 0;
}

