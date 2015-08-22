//
//  KVDBClient.h
//  testClient
//
//  Created by kimyongchan on 2015. 7. 27..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//

#ifndef __K_V_D_B_CLIENT_H__
#define __K_V_D_B_CLIENT_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "ClientServerPacket.h"
#include "ClientMasterPacket.h"
#include "Network.h"

enum SERVER_MODULE
{
    SERVER_MODULE_KVDB_SERVER = 1,
    SERVER_MODULE_MASTER_SERVER,
};


class KVDB : public ReceiveHandler
{
public:
    ClientServerPacket::BasicPacket* result;
    Network* network;
    int serverFd[10];
    
public:
    void Receive(const ConnectInfo* connectInfo, const char* data, int dataSize);
};

void KVDB::Receive(const ConnectInfo* connectInfo, const char *data, int dataSize)
{
    uint8_t commandType;
    memcpy(&commandType, data, sizeof(commandType));
    if(connectInfo->serverModule == SERVER_MODULE_MASTER_SERVER)
    {
        if(commandType == ClientMasterPacket::COMMAND_TYPE_SERVER_INFO_NOTIFY)
        {
            printf("ok");
            for(int i = 0; i < 2; i++)
            {
                char ip[16];
                memset(ip, 0, 16);
                memcpy(ip, data + sizeof(commandType) + sizeof(ClientMasterPacket::ServerInfo) * i, 16);
                uint16_t port;
                memcpy(&port, data + sizeof(commandType) + 16 + sizeof(ClientMasterPacket::ServerInfo) * i, sizeof(port));
                
                int fd = network->connectWithServer(SERVER_MODULE_KVDB_SERVER, ip, port);
                serverFd[i + 1] = fd;
                printf("%s %d %d \n", ip, port, fd);
            
            }
        }
    }
    else
    {
        printf("invalid commandType - %d", commandType);
    }
    
}

void KVDB_init(KVDB* conn)
{
    conn->result = NULL;
    conn->network = new Network();
    if(conn->network->initialize(conn) == false)
    {
        printf("network init error");
        return ;
    }
}

int KVDB_connect(KVDB* conn, const char* ip, const char* port, const char* id, const char* password)
{
    if((conn->serverFd[0] = conn->network->connectWithServer(SERVER_MODULE_MASTER_SERVER, ip, 3307)) < 0)
    {
        printf("can't create socket\n");
        return -1;
    }
    
    conn->network->receiveData(conn->serverFd[0]);
    
    return 1;
}

void KVDB_close(KVDB* conn)
{
    conn->network->disconnectWithServer(SERVER_MODULE_MASTER_SERVER);
    
}

void KVDB_sendQuery(KVDB* conn, const char* query, int queryLen)
{
    std::string queryStr(query);
    int index = (int)queryStr.find_first_of('\"');
    
    if(index < 0)
    {
        return ;
    }
    
    int sendServer = query[index+1] % 2;
    
    conn->network->sendPacket(conn->serverFd[sendServer], query, queryLen);
    
    if(conn->result != NULL)
    {
        
        switch (conn->result->resultType) {
            case ClientServerPacket::RESULT_TYPE_INSERT_FILE:
            {
                delete (ClientServerPacket::InsertFileRes*)conn->result;
                break;
            }
            case ClientServerPacket::RESULT_TYPE_INSERT_DIRECTORY:
            {
                delete (ClientServerPacket::InsertDirectoryRes*)conn->result;
                break;
            }
            case ClientServerPacket::RESULT_TYPE_FIND_FILE:
            {
                free (((ClientServerPacket::FindFileRes*)(conn->result))->value);
                delete (ClientServerPacket::FindFileRes*)conn->result;
                break;
            }
            case ClientServerPacket::RESULT_TYPE_FIND_DIRECTORY:
            {
                for(int i = 0; i < ((ClientServerPacket::FindDirectoryRes*)conn->result)->keyCount; i++)
                    free (((ClientServerPacket::FindDirectoryRes*)(conn->result))->keyInfo[i].key);
                delete (ClientServerPacket::FindDirectoryRes*)conn->result;
                break;
            }
            case ClientServerPacket::RESULT_TYPE_DELETE_FILE:
            {
                delete (ClientServerPacket::DeleteFileRes*)conn->result;
                break;
            }
            case ClientServerPacket::RESULT_TYPE_DELETE_DIRECTORY:
            {
                delete (ClientServerPacket::DeleteDirectoryRes*)conn->result;
                break;
            }
                
            default:
                break;
        }
    }
    
    char readBuf[2048];
    /*
    read(conn->s, )
    
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            memcpy(pSendBuffer, &insertFileRes->resultType, sizeof(insertFileRes->resultType));
            pSendBuffer += sizeof(insertFileRes->resultType);
            sendDataSize += sizeof(insertFileRes->resultType);
            
            memcpy(pSendBuffer, &insertFileRes->resultCode, sizeof(insertFileRes->resultCode));
            pSendBuffer += sizeof(insertFileRes->resultCode);
            sendDataSize += sizeof(insertFileRes->resultCode);
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);

        
        
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            memcpy(pSendBuffer, &insertDirectoryRes->resultType, sizeof(insertDirectoryRes->resultType));
            pSendBuffer += sizeof(insertDirectoryRes->resultType);
            sendDataSize += sizeof(insertDirectoryRes->resultType);
            
            memcpy(pSendBuffer, &insertDirectoryRes->resultCode, sizeof(insertDirectoryRes->resultCode));
            pSendBuffer += sizeof(insertDirectoryRes->resultCode);
            sendDataSize += sizeof(insertDirectoryRes->resultCode);
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
        
        
        
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            
            memcpy(pSendBuffer, &findFileRes->resultType, sizeof(findFileRes->resultType));
            pSendBuffer += sizeof(findFileRes->resultType);
            sendDataSize += sizeof(findFileRes->resultType);
            
            memcpy(pSendBuffer, &findFileRes->resultCode, sizeof(findFileRes->resultCode));
            pSendBuffer += sizeof(findFileRes->resultCode);
            sendDataSize += sizeof(findFileRes->resultCode);
            
            memcpy(pSendBuffer, &findFileRes->valueLen, sizeof(findFileRes->valueLen));
            pSendBuffer += sizeof(findFileRes->valueLen);
            sendDataSize += sizeof(findFileRes->valueLen);
            
            memcpy(pSendBuffer, findFileRes->value, findFileRes->valueLen);
            pSendBuffer += findFileRes->valueLen;
            sendDataSize += findFileRes->valueLen;
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
        
        
        
        
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            memcpy(pSendBuffer, &findDirectoryRes->resultType, sizeof(findDirectoryRes->resultType));
            pSendBuffer += sizeof(findDirectoryRes->resultType);
            sendDataSize += sizeof(findDirectoryRes->resultType);
            
            memcpy(pSendBuffer, &findDirectoryRes->resultCode, sizeof(findDirectoryRes->resultCode));
            pSendBuffer += sizeof(findDirectoryRes->resultCode);
            sendDataSize += sizeof(findDirectoryRes->resultCode);
            
            memcpy(pSendBuffer, &findDirectoryRes->keyCount, sizeof(findDirectoryRes->keyCount));
            pSendBuffer += sizeof(findDirectoryRes->keyCount);
            sendDataSize += sizeof(findDirectoryRes->keyCount);
            
            for(int i = 0; i < findDirectoryRes->keyCount; i++)
            {
                memcpy(pSendBuffer, &findDirectoryRes->keyInfo[i].fileType, sizeof(findDirectoryRes->keyInfo[i].fileType));
                pSendBuffer += sizeof(findDirectoryRes->keyInfo[i].fileType);
                sendDataSize += sizeof(findDirectoryRes->keyInfo[i].fileType);
                
                memcpy(pSendBuffer, &findDirectoryRes->keyInfo[i].keyLen, sizeof(findDirectoryRes->keyInfo[i].keyLen));
                pSendBuffer += sizeof(findDirectoryRes->keyInfo[i].keyLen);
                sendDataSize += sizeof(findDirectoryRes->keyInfo[i].keyLen);
                
                memcpy(pSendBuffer, findDirectoryRes->keyInfo[i].key, findDirectoryRes->keyInfo[i].keyLen);
                pSendBuffer += findDirectoryRes->keyInfo[i].keyLen;
                sendDataSize += findDirectoryRes->keyInfo[i].keyLen;
            }
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
        
        
        
        
        
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            memcpy(pSendBuffer, &deleteFileRes->resultType, sizeof(deleteFileRes->resultType));
            pSendBuffer += sizeof(deleteFileRes->resultType);
            sendDataSize += sizeof(deleteFileRes->resultType);
            
            memcpy(pSendBuffer, &deleteFileRes->resultCode, sizeof(deleteFileRes->resultCode));
            pSendBuffer += sizeof(deleteFileRes->resultCode);
            sendDataSize += sizeof(deleteFileRes->resultCode);
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
            
        
        
        
            char* pSendBuffer = sendBuffer;
            int sendDataSize = 0;
            memcpy(pSendBuffer, &deleteDirectoryRes->resultType, sizeof(deleteDirectoryRes->resultType));
            pSendBuffer += sizeof(deleteDirectoryRes->resultType);
            sendDataSize += sizeof(deleteDirectoryRes->resultType);
            
            memcpy(pSendBuffer, &deleteDirectoryRes->resultCode, sizeof(deleteDirectoryRes->resultCode));
            pSendBuffer += sizeof(deleteDirectoryRes->resultCode);
            sendDataSize += sizeof(deleteDirectoryRes->resultCode);
            
            
            KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);


    }
    */
}



#endif // __K_V_D_B_CLIENT_H__

