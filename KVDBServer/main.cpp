#include "KVDBServer.h"
#include "Log.h"
#include <stdlib.h>
#include <unistd.h>


int main(int argc, const char * argv[])
{

    if(KVDBServer::getInstance()->Initialize(1) == false)
    {
  
        ErrorLog("init error");
        return -1;     
    }
    
     KVDBServer::getInstance()->Run();
    
    return 0;
    
}







