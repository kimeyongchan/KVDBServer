//
//  LogBuffer.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "LogBuffer.h"

#include <stdlib.h>
#include <string.h>
#include <string>

#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"
#include "Log.h"

LogBuffer::LogBuffer()
{
    logLen = 0;
    logBuffer = NULL;
}

LogBuffer::~LogBuffer()
{
    free(logBuffer);
}

bool LogBuffer::initialize(int _cln)
{
    if(logBuffer == NULL)
        logBuffer = (char*)malloc(sizeof(char) * MAX_LOG_BUFFER_SIZE);
    
    logLen = 0;
    currentLogBufferSeek = logBuffer;
    cln = _cln;
    return true;
}

int LogBuffer::commitLogBuffer(char** pLogBuffer)
{
    if(logLen == 0)
    {
        return 0;
    }
    
    uint32_t commit = 0;
    memcpy(currentLogBufferSeek, &commit, sizeof(commit));
    currentLogBufferSeek += sizeof(commit);
    
    logLen += sizeof(commit);
    
    int sendLogLen = logLen;
    
    *pLogBuffer = (char*)malloc(logLen);
    memcpy(*pLogBuffer, logBuffer, logLen);
    
    logLen = 0;
    currentLogBufferSeek = logBuffer;
    cln++;
    
    return sendLogLen;
}

bool LogBuffer::saveLog(bool isAllocateBlock, bool isFreeBlock, bool isInsert, int64_t blockAddress, uint16_t freeSpace, uint16_t offsetLocation, uint16_t offset, const Data* data, int64_t prevBlockAddress, int64_t nextBlockAddress)
{
    
    char* tempSeek = currentLogBufferSeek;
    
    memcpy(currentLogBufferSeek, &cln, sizeof(cln));
    currentLogBufferSeek += sizeof(cln);
    
    
    ////////////////////////flag set
    
    char flag;
    memset(&flag, 0, 1);
    
    if(isAllocateBlock)
        flag |= IS_ALLOCATED_BLOCK;
    if(isFreeBlock)
        flag |= IS_FREE_BLOCK;
    if(isInsert)
        flag |= IS_INSERT;
    
    memcpy(currentLogBufferSeek, &flag, sizeof(flag));
    currentLogBufferSeek += sizeof(flag);
    
    if(isAllocateBlock || isFreeBlock)
    {
        memcpy(currentLogBufferSeek, &prevBlockAddress, sizeof(prevBlockAddress));
        currentLogBufferSeek += sizeof(prevBlockAddress);
        
        memcpy(currentLogBufferSeek, &nextBlockAddress, sizeof(nextBlockAddress));
        currentLogBufferSeek += sizeof(nextBlockAddress);
    }
    
    memcpy(currentLogBufferSeek, &blockAddress, sizeof(blockAddress));
    currentLogBufferSeek += sizeof(blockAddress);
    
    if(isFreeBlock)
    {
        //not logging data info
    }
    else
    {
        memcpy(currentLogBufferSeek, &freeSpace, sizeof(freeSpace));
        currentLogBufferSeek += sizeof(freeSpace);
        
        memcpy(currentLogBufferSeek, &offsetLocation, sizeof(offsetLocation));
        currentLogBufferSeek += sizeof(offsetLocation);
        
        memcpy(currentLogBufferSeek, &offset, sizeof(offset));
        currentLogBufferSeek += sizeof(offset);
        
        int8_t dataType = data->getFormatType();
        memcpy(currentLogBufferSeek, &dataType, sizeof(dataType));
        currentLogBufferSeek += sizeof(dataType);
        
        switch (dataType) {
            case FLAG_DIRECTORY_DATA:
            {
                const DirectoryData* dd = (DirectoryData*)data;
                
                uint8_t keyLen = dd->getKeyLength();
                memcpy(currentLogBufferSeek, &keyLen, sizeof(keyLen));
                currentLogBufferSeek += sizeof(keyLen);
                
                std::string key = dd->getKey();
                memcpy(currentLogBufferSeek, key.c_str(), keyLen);
                currentLogBufferSeek += keyLen;
                
                int64_t indBlockAddress = dd->getIndBlockAddress();
                memcpy(currentLogBufferSeek, &indBlockAddress, sizeof(indBlockAddress));
                currentLogBufferSeek += sizeof(indBlockAddress);
                
                break;
            }
            case FLAG_KEY_VALUE_DATA:
            {
                const KeyValueData* kd = (KeyValueData*)data;
                
                uint8_t keyLen = kd->getKeyLength();
                memcpy(currentLogBufferSeek, &keyLen, sizeof(keyLen));
                currentLogBufferSeek += sizeof(keyLen);
                
                std::string key = kd->getKey();
                memcpy(currentLogBufferSeek, key.c_str(), keyLen);
                currentLogBufferSeek += keyLen;
                
                uint32_t valueLen = kd->getValueLen();
                memcpy(currentLogBufferSeek, &valueLen, sizeof(valueLen));
                currentLogBufferSeek += sizeof(valueLen);
                
                std::string value = kd->getValue();
                memcpy(currentLogBufferSeek, value.c_str(), valueLen);
                currentLogBufferSeek += valueLen;
                
                break;
            }
            case FLAG_KEY_VALUE_CHAINING_DATA:
            {
                const KeyValueChainingData* kcd = (KeyValueChainingData*)data;
                
                uint8_t keyLen = kcd->getKeyLength();
                memcpy(currentLogBufferSeek, &keyLen, sizeof(keyLen));
                currentLogBufferSeek += sizeof(keyLen);
                
                std::string key = kcd->getKey();
                memcpy(currentLogBufferSeek, key.c_str(), keyLen);
                currentLogBufferSeek += keyLen;
                
                uint32_t valueLen = kcd->getValueLen();
                memcpy(currentLogBufferSeek, &valueLen, sizeof(valueLen));
                currentLogBufferSeek += sizeof(valueLen);
                
                std::string value = kcd->getValue();
                memcpy(currentLogBufferSeek, value.c_str(), valueLen);
                currentLogBufferSeek += valueLen;
                
                int64_t chainingBA = kcd->getIndBlockAddress();
                memcpy(currentLogBufferSeek, &chainingBA, sizeof(chainingBA));
                currentLogBufferSeek += sizeof(chainingBA);
                
                break;
            }
                
            default:
            {
                ErrorLog("invalid type - %d", data->getFormatType());
                memset(tempSeek, 0, currentLogBufferSeek - tempSeek);
                currentLogBufferSeek = tempSeek;
                return false;
            }
        }
    }

    logLen += currentLogBufferSeek - tempSeek;
    
    return true;
}



