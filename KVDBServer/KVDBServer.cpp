#include "KVDBServer.h"

#include "Defines.h"
#include "Network.h"
#include "RequestInfo.h"
#include "XmlData.h"
#include "ClientRequestHandler.h"
#include "MasterRequestHandler.h"
#include "ClientResponseHandler.h"
#include "MasterResponseHandler.h"
#include "WorkerThread.h"
#include "DiskManager.h"
#include "IOManager.h"

#include "LogBuffer.h"
#include "LogFile.h"

#include "Log.h"

#define PORT 3307

KVDBServer* KVDBServer::m_instance = NULL;

bool KVDBServer::Initialize(int workerThreadCount)
{
    m_log = new Log();
    if (m_log->Initialize(".") == false)
    {
        return false;
    }
    
    xmlData = new XmlData();
    if(xmlData->initialize() == false)
    {
        ErrorLog("xmlData error");
        return false;
    }
    
    MasterRequestHandler* masterRequestHandler = new MasterRequestHandler();
    if (masterRequestHandler->Initialize() == false)
    {
        ErrorLog("request error");
        return false;
    }
    
    ClientRequestHandler* clientRequestHandler = new ClientRequestHandler();
    if (clientRequestHandler->Initialize() == false)
    {
        ErrorLog("request error");
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
        /*
        switch (networkInfoList[i].module) {
            case SERVER_MODULE_MASTER:
            {
                networkInfoList[i].receiveHandler = masterRequestHandler;
                break;
            }
            case SERVER_MODULE_CLIENT:
            {
                networkInfoList[i].receiveHandler = clientRequestHandler;
                break;
            }
            default:
                break;
        }*/
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
    
    
    
    masterResponseHandler = new MasterResponseHandler();
    if(masterResponseHandler->initialize() == false)
    {
        ErrorLog("master res init error");
        return false;
    }
    
    clientResponseHandler = new ClientResponseHandler();
    if(clientResponseHandler->initialize() == false)
    {
        ErrorLog("client res init error");
        return false;
    }
    
    m_logBuffer = new LogBuffer();
    if (m_logBuffer->initialize() == false)
    {
        return false;
    }
    
    m_logFile = new LogFile();
    if (m_logFile->initialize(KVDB_LOG_PATH) == false)
    {
        return false;
    }
    
    m_diskManager = new DiskManager();
    if (m_diskManager->initialize(KVDB_PATH, BLOCK_SIZE, DISK_SIZE) == false)
    {
        ErrorLog("diskManager error");
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
/*
void KVDBServer::SendWorkToWorkerThread(RequestInfo* ri)
{
    int firstCnt, secondCnt;
    int fitNum;
    
    firstCnt = m_workerThreadArray[0].GetRequestInfoCount();
    
    if(firstCnt == 0)
    {
        m_workerThreadArray[0].PushRequestInfo(ri);
        return ;
    }
    
    fitNum = 0;
    
    for(int i=1; i < m_workerThreadCount - 1; i++)
    {
        secondCnt = m_workerThreadArray[i].GetRequestInfoCount();
        
        if(secondCnt == 0)
        {
            m_workerThreadArray[i].PushRequestInfo(ri);
            return ;
        }
        
        if(firstCnt > secondCnt)
        {
            secondCnt = firstCnt;
            fitNum = i;
        }
    }
    
    m_workerThreadArray[fitNum].PushRequestInfo(ri);
        
    return ;
}
*/

