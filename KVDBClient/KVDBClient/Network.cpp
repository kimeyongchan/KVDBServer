//
//  Network.cpp
//  KVDBClient
//
//  Created by kimyongchan on 2015. 8. 20..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "Network.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>


////////////////////////TempBufferInfo
TempBufferInfo::TempBufferInfo()
{
    tempBufferSize = 0;
    tempBuffer = NULL;
}

TempBufferInfo::~TempBufferInfo()
{
    free(tempBuffer);
}

void TempBufferInfo::clear()
{
    tempBufferSize = 0;
    free(tempBuffer);
    tempBuffer = NULL;
}

bool TempBufferInfo::isEmpty()
{
    if(tempBufferSize == 0)
        return true;
    else
        return false;
}



////////////////////////ConnectInfo
ConnectInfo::ConnectInfo()
{
    fd = 0;
    serverModule = 0;
    tempBufferInfo.clear();
}

ConnectInfo::~ConnectInfo()
{
    tempBufferInfo.clear();
}

void ConnectInfo::clear()
{
    fd = 0;
    serverModule = 0;
    tempBufferInfo.clear();
}



Network::Network()
{
    
}


Network::~Network()
{
    
}


bool Network::initialize(ReceiveHandler* _receiveHandler)
{
    for(int i = 0; i <MAX_FD_COUNT; i++)
    {
        fdArray[i].clear();
    }
    
    receiveHandler = _receiveHandler;
    
    return true;
}


int Network::connectWithServer(int serverModule, const char* ip, unsigned short port)
{
    int i;
    for(i = 0; i < MAX_FD_COUNT; i++)
    {
        if(fdArray[i].fd <= 0)
            break;
    }
    
    
    if(i == MAX_FD_COUNT) return -1;
    
    
    int sock;
    struct sockaddr_in addr;
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("socket() failed.\n");
        return -2;
    }
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("can't connect.\n");
        return -3;
    }
    
    fdArray[i].fd = sock;
    fdArray[i].serverModule = serverModule;
    fdArray[i].tempBufferInfo.tempBufferSize = 0;
    fdArray[i].tempBufferInfo.tempBuffer = NULL;
    

    return fdArray[i].fd;
}


bool Network::receiveData(int fd)
{
    ConnectInfo* connectInfo = getConnectInfo(fd);
    
    if(connectInfo == NULL)
    {
        return false;
    }
    
    int readCnt = (int)recv(connectInfo->fd, recvBuffer, MAX_BUFFER_LEN, NULL);
    
    if(readCnt > 0)
    {
        char* wholeRecvBuffer = recvBuffer;
        int wholeRecvBufferSize = readCnt;
        int recvCnt = readCnt;
        char* allocateRecvBuffer = NULL;
            
            
        TempBufferInfo* tempBufferInfo = &connectInfo->tempBufferInfo;
            
        int tempBufferSize = tempBufferInfo->tempBufferSize;
            
            
        if(tempBufferSize != 0)
        {
            wholeRecvBuffer = (char*)malloc(sizeof(char) * (recvCnt + tempBufferSize));
                
            memcpy(wholeRecvBuffer, tempBufferInfo->tempBuffer, tempBufferSize);
            memcpy(wholeRecvBuffer + tempBufferSize, recvBuffer, recvCnt);
            wholeRecvBufferSize += tempBufferInfo->tempBufferSize;
                
            connectInfo->tempBufferInfo.clear();
                
            allocateRecvBuffer = wholeRecvBuffer;
        }
            
        char* pWholeRecvBuffer = wholeRecvBuffer;
        int pWholeRecvBufferSize = wholeRecvBufferSize;
            
        while(true)
        {
            if(pWholeRecvBufferSize < sizeof(dataType_t))
            {
                connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(sizeof(char) * wholeRecvBufferSize);
                memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                
                break;
            }
                
            ////////////////////////// data type
            dataType_t flags;
            memcpy(&flags, pWholeRecvBuffer, sizeof(flags));
            pWholeRecvBuffer += sizeof(flags);
            pWholeRecvBufferSize -= sizeof(flags);
            
            if(flags == DATA_TYPE_PING_NOTIFY)
            {
                dataType_t dataTypeToSend = DATA_TYPE_PING_OK;
                sendData(connectInfo->fd, (char*)&dataTypeToSend, sizeof(dataTypeToSend));
                    
                if(pWholeRecvBufferSize == 0)
                {
                    break;
                }
                else if(pWholeRecvBufferSize > 0) // 남은 데이터들 존재함
                {
                    wholeRecvBufferSize = pWholeRecvBufferSize;
                    wholeRecvBuffer = pWholeRecvBuffer;
                        
                    continue;
                }
                else
                {
                    printf("pWholeRecvBufferSize - %d", pWholeRecvBufferSize);
                    return false;
                }
            }
            else if(flags == DATA_TYPE_REQ)
            {
                if(pWholeRecvBufferSize < sizeof(dataSize_t))
                {
                    connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                    connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(wholeRecvBufferSize);
                    memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                        
                    break;
                }
                    
                dataSize_t dataSize;
                memcpy(&dataSize, pWholeRecvBuffer, sizeof(dataSize));
                pWholeRecvBuffer += sizeof(dataSize);
                pWholeRecvBufferSize -= sizeof(dataSize);
                    
                    
                if(dataSize <= pWholeRecvBufferSize)
                {
                    receiveHandler->Receive(connectInfo, pWholeRecvBuffer, (int)dataSize);
                    
                    pWholeRecvBuffer += dataSize;
                    pWholeRecvBufferSize -= dataSize;
                        
                    if(pWholeRecvBufferSize == 0)
                    {
                        break;
                    }
                    else if(pWholeRecvBufferSize > 0)
                    {
                        wholeRecvBufferSize = pWholeRecvBufferSize;
                        wholeRecvBuffer = pWholeRecvBuffer;
                            
                        continue;
                    }
                    else
                    {
                        printf("pWholeRecvBufferSize - %d", pWholeRecvBufferSize);
                    }
                }
                else
                {
                    connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                    connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(wholeRecvBufferSize);
                    memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                    
                    break;
                }
            }
            
        }
            
        if(allocateRecvBuffer != NULL)
        {
            free(allocateRecvBuffer);
        }
    }
    else if (readCnt == 0)
    {
        printf("readCnt %d", readCnt);
    }
    else
    {
        printf("readCnt %d", readCnt);
    }
    
    return true;
}


bool Network::sendPacket(int fd, const char* data, int dataSize)
{
    dataHeader.dataType = DATA_TYPE_REQ;
    dataHeader.dataSize = dataSize;
    
    char packetArray[sizeof(dataHeader) + dataSize];
    memcpy(packetArray, &dataHeader, sizeof(dataHeader));
    memcpy(packetArray + sizeof(dataHeader), data, dataSize);
    
    if(sendData(fd, packetArray, sizeof(dataHeader) + dataSize) == false)
    {
        return false;
    }
    
    return true;
}


bool Network::sendData(int fd, const char* data, int dataSize)
{
    send(fd, data, dataSize, NULL);
    
    return true;
}


bool Network::disconnectWithServer(int fd)
{
    ConnectInfo* connectInfo = getConnectInfo(fd);
    
    if(connectInfo == NULL)
    {
        return false;
    }

    close(connectInfo->fd);
    
    connectInfo->clear();
    
    return true;
}


ConnectInfo* Network::getConnectInfo(int fd)
{
    for(int i = 0; i < MAX_FD_COUNT; i++)
    {
        if(fdArray[i].fd == fd)
        {
            return &fdArray[i];
        }
    }
    
    return NULL;
}




