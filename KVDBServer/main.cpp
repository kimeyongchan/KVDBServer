#include "KVDBServer.h"
#include "Log.h"

int main(int argc, const char * argv[]) {
    
    if(KVDBServer::getInstance()->Initialize(4) == false)
    {
        ErrorLog("init error");
        return -1;
    }

    KVDBServer::getInstance()->Run();
    
    
    
    return 0;
}