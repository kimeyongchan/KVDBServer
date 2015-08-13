#include "MasterServer.h"
#include "Log.h"

int main(int argc, const char * argv[]) {
    
    if(MasterServer::getInstance()->Initialize(4) == false)
    {
        ErrorLog("init error");
        return -1;
    }
    
    MasterServer::getInstance()->Run();
    
    return 0;
}
