#include "IOManager.h"

#include "RequestInfo.h"
#include "Log.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}


void IOManager::ProcessInsert(InsertRequestInfo* reqInfo)
{
    
    DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());
    
}


void IOManager::ProcessInsert(InsertDirectoryRequestInfo* reqInfo)
{
    
    DebugLog("INSERT_DIRECTORY - key : %s", reqInfo->key.c_str());
    
}


void IOManager::ProcessFind(FindRequestInfo* reqInfo)
{
    
    DebugLog("FIND - key : %s", reqInfo->key.c_str());
    
}


void IOManager::ProcessDelete(DeleteRequestInfo* reqInfo)
{
    
    DebugLog("DELETE - key : %s", reqInfo->key.c_str());
    
}