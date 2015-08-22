#include "MasterServer.h"

#include "Defines.h"
#include "Network.h"
#include "XmlData.h"
#include "IOManager.h"

#include "Log.h"

#define PORT 3307

MasterServer* MasterServer::m_instance = NULL;

bool MasterServer::Initialize(int workerThreadCount)
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
    
    WorkerThread** workerThreadArray = new WorkerThread*[workerThreadCount];
    
    for(int i = 0; i < workerThreadCount; i++)
    {
        workerThreadArray[i] = new IOManager();
    }
    
    network = new Network();
    if (network->Initialize(networkInfoList, networkInfoCount, workerThreadCount, workerThreadArray, 0, 0) == false)
    {
        ErrorLog("Network error");
        return false;
    }
    slaveInfo[0].used = 0;
    slaveInfo[1].used = 0;
    
    slaveInfo[0].port = 20000;
    slaveInfo[1].port = 20001;

	return true;
}

void MasterServer::Run()
{
	while (true)
	{
		network->ProcessEvent();
        
	}
}


