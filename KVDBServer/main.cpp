#include "KVDBServer.h"
#include "Log.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "IOManager.h"
#include "RequestInfo.h"



int main(int argc, const char * argv[])
{

    if(KVDBServer::getInstance()->Initialize(1) == false)
    {
  
        ErrorLog("init error");
        return -1;     
    }
    

    // KVDBServer::getInstance()->Run();
    
    InsertDirectoryRequestInfo reqDirInfo_a;
    reqDirInfo_a.key ="a";
    
    
    InsertDirectoryRequestInfo reqDirInfo_ab;
    reqDirInfo_ab.key ="a/b";
    
    
    InsertDirectoryRequestInfo reqDirInfo_abc;
    reqDirInfo_abc.key ="a/b/c";
    
    
    InsertRequestInfo reqInfo;
    reqInfo.key ="a/b/c/d";
    reqInfo.value = "he";

    
    IOManager ioMgr;
    ioMgr.processInsert(&reqDirInfo_a);
    ioMgr.processInsert(&reqDirInfo_ab);
    ioMgr.processInsert(&reqDirInfo_abc);
    ioMgr.processInsert(&reqInfo);

    //FindRequestInfo findReqInfo;
   // findReqInfo.key = "a/b/c/d";
    
    //ioMgr.processFind(&findReqInfo);
    
     DeleteRequestInfo delReqInfo;
     delReqInfo.key = "a/b/c/d";
    
    ioMgr.processDelete(&delReqInfo);
    
  //  ioMgr.processFind(&findReqInfo);
    
    
    return 0;
    
}







