#include "KVDBServer.h"

#include "Defines.h"
#include "Network.h"
#include "XmlData.h"
#include "IOManager.h"
#include "DiskManager.h"
#include "LogBuffer.h"
#include "LogFile.h"
#include "SuperBlock.h"
#include "NamedCache.h"
#include "bufferCache.h"

#include "Log.h"

#define PORT 3307

KVDBServer* KVDBServer::m_instance = NULL;

bool KVDBServer::Initialize(int workerThreadCount)
{
    log = new Log();
    if (log->Initialize(".") == false)
    {
        return false;
    }
    
    xmlData = new XmlData();
    if(xmlData->initialize() == false)
    {
        ErrorLog("xmlData error");
        return false;
    }

    int networkInfoCount = xmlData->serverInfoCount;
    
    NetworkInfo networkInfoList[networkInfoCount];
    memset(networkInfoList, 0, sizeof(NetworkInfo) * networkInfoCount);
    
    for(int i = 0; i < networkInfoCount; i++)
    {
        networkInfoList[i].type = xmlData->serverInfoList[i].serverType;
        networkInfoList[i].module = xmlData->serverInfoList[i].serverModule;
        memcpy(networkInfoList[i].ip, xmlData->serverInfoList[i].ip, MAX_IP_LEN);
        networkInfoList[i].port = xmlData->serverInfoList[i].port;
    }
    
    WorkerThread* workerThreadArray = new IOManager[workerThreadCount];
    
    for(int i = 0; i < workerThreadCount; i++)
    {
        if(workerThreadArray[i].Initialize() == false)
        {
            ErrorLog("workerThread error");
            return false;
        }
    }
    
    network = new Network();
    if (network->Initialize(networkInfoList, networkInfoCount, workerThreadCount, workerThreadArray) == false)
    {
        ErrorLog("Network error");
        return false;
    }
    
    logBuffer = new LogBuffer();
    if (logBuffer->initialize() == false)
    {
        return false;
    }
    
    logFile = new LogFile();
    if (logFile->initialize(KVDB_LOG_NAME) == false)
    {
        return false;
    }
    
    superBlock = new SuperBlock();
    
    diskManager = new DiskManager();
    if (diskManager->initialize(KVDB_NAME, BLOCK_SIZE, DISK_SIZE, superBlock) == false)
    {
        ErrorLog("diskManager error");
        return false;
    }
    
    bc = new BufferCache(superBlock);
    nc = new NamedCache(superBlock);

	return true;
}

void KVDBServer::Run()
{
	while (true)
	{
		network->ProcessEvent();
	}
}


