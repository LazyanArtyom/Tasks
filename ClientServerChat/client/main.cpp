#include "client.h"

int main()
{
   messaging::Client client("127.0.0.1", 4444);
   client.connect();

   return 0;
}