//
//  ClientResponseHandler.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 19..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "ClientResponseHandler.h"
#include "KVDBServer.h"
#include "Network.h"

#include <string.h>

void ClientResponseHandler::InsertFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::InsertFileRes* insertFileRes)
{
    char* pSendBuffer = sendBuffer;
    int sendDataSize = 0;
    memcpy(pSendBuffer, &insertFileRes->resultType, sizeof(insertFileRes->resultType));
    pSendBuffer += sizeof(insertFileRes->resultType);
    sendDataSize += sizeof(insertFileRes->resultType);
    
    memcpy(pSendBuffer, &insertFileRes->resultCode, sizeof(insertFileRes->resultCode));
    pSendBuffer += sizeof(insertFileRes->resultCode);
    sendDataSize += sizeof(insertFileRes->resultCode);
    
    
    KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
}


void ClientResponseHandler::InsertDirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::InsertDirectoryRes* insertDirectoryRes)
{
    char* pSendBuffer = sendBuffer;
    int sendDataSize = 0;
    memcpy(pSendBuffer, &insertDirectoryRes->resultType, sizeof(insertDirectoryRes->resultType));
    pSendBuffer += sizeof(insertDirectoryRes->resultType);
    sendDataSize += sizeof(insertDirectoryRes->resultType);
    
    memcpy(pSendBuffer, &insertDirectoryRes->resultCode, sizeof(insertDirectoryRes->resultCode));
    pSendBuffer += sizeof(insertDirectoryRes->resultCode);
    sendDataSize += sizeof(insertDirectoryRes->resultCode);
    
    
    KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);
}


void ClientResponseHandler::FindFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::FindFileRes* findFileRes)
{
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
}


void ClientResponseHandler::FindRirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::FindDirectoryRes* findDirectoryRes)
{
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
}


void ClientResponseHandler::DeleteFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::DeleteFileRes* deleteFileRes)
{
    char* pSendBuffer = sendBuffer;
    int sendDataSize = 0;
    memcpy(pSendBuffer, &deleteFileRes->resultType, sizeof(deleteFileRes->resultType));
    pSendBuffer += sizeof(deleteFileRes->resultType);
    sendDataSize += sizeof(deleteFileRes->resultType);
    
    memcpy(pSendBuffer, &deleteFileRes->resultCode, sizeof(deleteFileRes->resultCode));
    pSendBuffer += sizeof(deleteFileRes->resultCode);
    sendDataSize += sizeof(deleteFileRes->resultCode);
    
    
    KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendBuffer, sendDataSize);

}


void ClientResponseHandler::DeleteDirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::DeleteDirectoryRes* deleteDirectoryRes)
{
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


