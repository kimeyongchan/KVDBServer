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
    if (network->Initialize(networkInfoList, networkInfoCount, workerThreadCount, workerThreadArray, 5000, 3) == false)
    {
        ErrorLog("Network error");
        return false;
    }

	return true;
}

void MasterServer::Run()
{
	while (true)
	{
		network->ProcessEvent();
        
	}
}


