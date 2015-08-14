#include "IOManager.h"
#include "XmlData.h"
#include "Log.h"
#include "MasterServer.h"
#include "Defines.h"
#include "Network.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}

void enterConnect(const ConnectInfo* connectInfo)
{
    
}


void leaveConnect(const ConnectInfo* connectInfo)
{
    
}


void IOManager::receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{    
    if(connectInfo->serverModule == SERVER_MODULE_SLAVE)
    {
        receiveSlaveData(connectInfo, data, dataSize);
    }
    else if(connectInfo->serverModule == SERVER_MODULE_CLIENT)
    {
        receiveClientData(connectInfo, data, dataSize);
    }
    else
    {
        ErrorLog("invalid server module - %d", connectInfo->serverModule);
    }
}


void IOManager::receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{

}


void IOManager::receiveSlaveData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    
}