#include "KVDBServer.h"

#include "Defines.h"
#include "Network.h"
#include "XmlData.h"
#include "IOManager.h"
#include "DiskManager.h"
#include "LogBuffer.h"
#include "LogFile.h"
#include "SuperBlock.h"
#include "Log.h"
#include "LogInfo.h"
#include <deque>

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
    if (network->Initialize(networkInfoList, networkInfoCount, workerThreadCount, workerThreadArray, 1000, 3) == false)
    {
        ErrorLog("Network error");
        return false;
    }
    
    logFile = new LogFile();
    if (logFile->initialize(KVDB_LOG_NAME) == false)
    {
        return false;
    }

    
    
    logBuffer = new LogBuffer();
    if (logBuffer->initialize(logFile->getCln() + 1) == false)
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
    
    // cache에 슈퍼블럭 주기

    if(superBlock->getCln() < logFile->getCln()) // recovery
    {
        std::deque<LogInfo*> dequeue;
        
        if(logFile->recoveryLogFile(superBlock->getCln(), &dequeue) == false)
        {
            ErrorLog("not recovery");
            return false;
        }
        
        while(1)
        {
            if(dequeue.empty())
            {
                break;
            }
            else
            {
                if(diskManager->recovery(dequeue.front()) == false)
                {
                    ErrorLog("disk recovery error");
                    return false;
                }
                dequeue.pop_front();
            }
        }
        
        //diskManager->setCln(logFilecln);
    }
    else if(superBlock->getCln() == logFile->getCln()) // not recovery
    {
        
    }
    else //error
    {
        ErrorLog("disk cln - %d, logfile cln - %d", superBlock->getCln(), logFile->getCln());
        return false;
    }
    
    
	return true;
}

void KVDBServer::Run()
{
	while (true)
	{
		network->ProcessEvent();
	}
}


