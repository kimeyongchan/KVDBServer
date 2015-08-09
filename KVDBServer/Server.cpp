#include "Server.h"

#include <string.h>

#include "Network.h"
#include "MasterRequestHandler.h"
#include "ClientRequestHandler.h"
#include "MasterResponseHandler.h"
#include "ClientResponseHandler.h"
#include "XmlData.h"

#include "Log.h"

Server* Server::instance = NULL;


bool Server::Initialize(int workerThreadCount)
{
/*	log = new Log();
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
        
        switch (networkInfoList[i].module) {
            case SERVER_MODULE_MASTER:
            {
                networkInfoList[i].receiveHandler = masterRequestHandler;
                break;
            }
            case SERVER_MODULE_SLAVE:
            {
                networkInfoList[i].receiveHandler = slaveRequestHandler;
                break;
            }
            case SERVER_MODULE_CLIENT:
            {
                networkInfoList[i].receiveHandler = clientRequestHandler;
                break;
            }
            default:
                break;
        }
    }

	network = new Network();
	if (network->Initialize(networkInfoList, networkInfoCount) == false)
	{
		ErrorLog("Network error");
		return false;
	}
    
    masterResponseHandler = new MasterResponseHandler();
    if(masterResponseHandler->initialize() == false)
    {
        ErrorLog("masterResponseHandler->Initialize() error");
        return false;
    }
    
    slaveResponseHandler = new SlaveResponseHandler();
    if(slaveResponseHandler->initialize() == false)
    {
        ErrorLog("slaveResponseHandler->Initialize() error");
        return false;
    }
    
    clientResponseHandler = new ClientResponseHandler();
    if(clientResponseHandler->initialize() == false)
    {
        ErrorLog("clientResponseHandler->Initialize() error");
        return false;
    }
    */
	return true;
}

void Server::Run()
{
	while (true)
	{
		network->ProcessEvent();
	}
}