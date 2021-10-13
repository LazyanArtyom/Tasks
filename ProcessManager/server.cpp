#include "include/processmanager.h"

#define SERVER_FIFO "/tmp/addition_fifo_server"
#define MAX_NUMBERS 500

int main()
{
   procmgr::ProcessManager procMgr;
   procMgr.exec();

   return 0; 
}