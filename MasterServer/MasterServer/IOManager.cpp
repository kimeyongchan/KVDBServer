#include "IOManager.h"
#include "XmlData.h"
#include "Log.h"
#include "MasterServer.h"
#include "Defines.h"
#include "Network.h"
#include "ServerMasterPacket.h"
#include "ClientMasterPacket.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}

void IOManager::connected(const ConnectInfo* connectInfo)
{
    DebugLog("connected");
    if(connectInfo->serverModule == SERVER_MODULE_SLAVE)
    {
        for(int i = 0; i < 2; i++)
        {
            if(MasterServer::getInstance()->slaveInfo[i].used ==0)
            {
                DebugLog("slave send");
                MasterServer::getInstance()->slaveInfo[i].used = 1;
                ServerMasterPacket::OpenClientSockReq ocsr;
                
                memcpy(&ocsr.port, &MasterServer::getInstance()->slaveInfo[i].port, sizeof(uint16_t));
                char sendArray[sizeof(ocsr)];
                memcpy(sendArray, &ocsr, sizeof(ocsr));
                MasterServer::getInstance()->network->sendData(tfd, connectInfo, sendArray, sizeof(ocsr));
                return;
            }
        }
    }
    else if(connectInfo->serverModule == SERVER_MODULE_CLIENT)
    {
        ClientMasterPacket::ServerInfoNotify sin;
        
        for(int i = 0; i < 2; i++)
        {
            if(MasterServer::getInstance()->slaveInfo[i].used == 1)
            {
                DebugLog("client send");
                memcpy(&sin.serverInfo[i].ip, &MasterServer::getInstance()->slaveInfo[i].ip, 16);
                sin.serverInfo[i].port = MasterServer::getInstance()->slaveInfo[i].port;
            }
            else
            {
                ErrorLog("not server open");
                return ;
            }
        }
        
        char sendArray[sizeof(sin)];
        memcpy(sendArray, &sin, sizeof(sin));
        MasterServer::getInstance()->network->sendData(tfd, connectInfo, sendArray, sizeof(sin));
        return;
    }
    
    
    
    MasterServer::getInstance()->network->finishProcessing(tfd, connectInfo);
}


void IOManager::disconnected(const ConnectInfo* connectInfo)
{
    DebugLog("disconnected");
    
    MasterServer::getInstance()->network->finishProcessing(tfd, connectInfo);
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
    uint8_t commandType;
    memcpy(&commandType, data, sizeof(commandType));
    
    if(commandType == ServerMasterPacket::COMMAND_TYPE_OPEN_CLIENT_SOCK_RES)
    {
        uint16_t port;
        memcpy(&port, data + sizeof(commandType), sizeof(port));
        
        for(int i = 0; i < 3; i++)
        {
            if(MasterServer::getInstance()->slaveInfo[i].used == 1 && MasterServer::getInstance()->slaveInfo[i].port == port)
            {
                DebugLog("slave recv");
                MasterServer::getInstance()->slaveInfo[i].used = 1;
                memcpy(MasterServer::getInstance()->slaveInfo[i].ip, data + sizeof(commandType) + sizeof(port), 16);
                DebugLog("ip %s", MasterServer::getInstance()->slaveInfo[i].ip);
            }
        }
    }
}