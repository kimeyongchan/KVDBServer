//
//  LogFile.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "LogFile.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Log.h"
#include "LogInfo.h"
#include "LogBuffer.h"

#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"

LogFile::LogFile()
{
    
}

bool LogFile::initialize(const char* fileName)
{
    char filePath[1000] = {0, };
    memcpy(filePath, __FILE__, strlen(__FILE__));
    int slushCount = 0;
    for(int i = (int)strlen(filePath); i > 0; i--)
    {
        if(filePath[i] == '/')
        {
            slushCount++;
            if(slushCount == 2)
            {
                memcpy(filePath + i + 1, fileName, strlen(fileName));
                filePath[i+1+strlen(fileName)] = '\0';
                break;
            }
        }
    }
        
    if ((fd = open(filePath, O_RDWR | O_SYNC)) < 0) // if not exist disk
    {
        if(createDisk(filePath) == false)
        {
            ErrorLog("create Disk fail");
            return false;
        }
    }
    else
    {
        lseek(fd, 0, SEEK_SET);
        
        if(read(fd, &cln, sizeof(cln)) < 0)
        {
            ErrorLog("read block error");
            return false;
        }
        
        logFileSize = sizeof(cln);
    }
    
    
    
    return true;
}


bool LogFile::createDisk(const char* fileName)
{
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC , 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file create error");
        return false;
    }
    
    if(ftruncate(fd, MAX_LOG_FILE_SIZE) < 0)
    {
        ErrorLog("ftruncate error");
        return false;
    }
    
    lseek(fd, 0, SEEK_SET);
    
    cln = 0;
    
    if(write(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    logFileSize = sizeof(cln);
    
    return true;
}

bool LogFile::writeLogFile(int logSize, const char* logArray)
{
    lseek(fd, logFileSize, SEEK_SET);
    
    if(write(fd, logArray, logSize) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    logFileSize += logSize;
    
    cln++;
    
    lseek(fd, 0, SEEK_SET);
    
    if(write(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write cln error");
        return false;
    }

    return true;
}

bool LogFile::recoveryLogFile(int diskCln, std::deque<LogInfo*>* dequeue)
{
    lseek(fd, sizeof(cln), SEEK_SET);

    char logArray[1024 * 1024];
    
    if(read(fd, logArray, 1024 * 1024) < 0)
    {
        ErrorLog("read block error");
        return -1;
    }
    
    char* pLogArray = logArray;
    uint32_t lastLogScn = diskCln;
    uint32_t logScn;
    while(1)
    {
        memcpy(pLogArray, &logScn, sizeof(logScn));
        pLogArray += sizeof(logScn);
        
        if(logScn == 0) // commit
        {
            if(lastLogScn + 1 == cln)
            {
                break;
            }
            else
            {
                lastLogScn++;
                continue;
            }
        }
        
        DebugLog("diskcln = %d, logfile cln = %d, last scn = %d, scn = %d", diskCln, cln, lastLogScn, logScn);
        
        LogInfo* logInfo = new LogInfo();
        memset(logInfo, 0, sizeof(LogInfo));
        
        int8_t flags;
        memcpy(&flags, pLogArray, sizeof(flags));
        pLogArray += sizeof(flags);
        
        if((flags &= IS_ALLOCATED_BLOCK) != 0)
        {
            logInfo->isAllocateBlock = true;
        }
        else if((flags &= IS_FREE_BLOCK) != 0)
        {
            logInfo->isFreeBlock = true;
        }
        
        if((flags &= IS_INSERT) != 0)
        {
            logInfo->isInsert = true;
        }
        
        if(logInfo->isFreeBlock || logInfo->isAllocateBlock)
        {
            memcpy(&logInfo->prevBlockAddress, pLogArray, sizeof(logInfo->prevBlockAddress));
            pLogArray += sizeof(logInfo->prevBlockAddress);
            
            memcpy(&logInfo->nextBlockAddress, pLogArray, sizeof(logInfo->nextBlockAddress));
            pLogArray += sizeof(logInfo->nextBlockAddress);
        }
        
        memcpy(&logInfo->blockAddress, pLogArray, sizeof(logInfo->blockAddress));
        pLogArray += sizeof(logInfo->blockAddress);
        
        if(logInfo->isFreeBlock)
        {
            logInfo->freeSpace = 0;
            logInfo->offsetLocation = 0;
            logInfo->offset = 0;
            logInfo->data = NULL;
        }
        else
        {
            memcpy(&logInfo->freeSpace, pLogArray, sizeof(logInfo->freeSpace));
            pLogArray += sizeof(logInfo->freeSpace);
            
            memcpy(&logInfo->offsetLocation, pLogArray, sizeof(logInfo->offsetLocation));
            pLogArray += sizeof(logInfo->offsetLocation);

            memcpy(&logInfo->offset, pLogArray, sizeof(logInfo->offset));
            pLogArray += sizeof(logInfo->offset);
            
            int8_t dataType;
            memcpy(&dataType, pLogArray, sizeof(dataType));
            pLogArray += sizeof(dataType);
            
            switch (dataType) {
                case FLAG_DIRECTORY_DATA:
                {
                    DirectoryData* dd = new DirectoryData();
                    dd->setFormatType(dataType);
                    
                    
                    
                    uint8_t keyLen;
                    memcpy(&keyLen, pLogArray, sizeof(keyLen));
                    pLogArray += sizeof(keyLen);
                    
                    
                    char key[keyLen + 1];
                    memset(key, 0, keyLen);
                    
                    memcpy(key, pLogArray, keyLen);
                    pLogArray += keyLen;
                    
                    std::string keyStr(key);
                    DebugLog("%s", keyStr.c_str());
                    dd->setKey(key);
                    
                    int64_t indBlockAddress;
                    memcpy(&indBlockAddress, pLogArray, sizeof(indBlockAddress));
                    pLogArray += sizeof(indBlockAddress);
                    
                    dd->setIndBlockAddress(indBlockAddress);
                    
                    logInfo->data = dd;
                    
                    break;
                }
                case FLAG_KEY_VALUE_DATA:
                {
                    KeyValueData* kd = new KeyValueData();
                    
                    uint8_t keyLen;
                    memcpy(&keyLen, pLogArray, sizeof(keyLen));
                    pLogArray += sizeof(keyLen);
                    
                    
                    char key[keyLen + 1];
                    memset(key, 0, keyLen);
                    
                    memcpy(key, pLogArray, keyLen);
                    pLogArray += keyLen;
                    
                    std::string keyStr(key);
                    DebugLog("%s", keyStr.c_str());
                    kd->setKey(key);
                    
                    
                    uint8_t valueLen;
                    memcpy(&valueLen, pLogArray, sizeof(valueLen));
                    pLogArray += sizeof(valueLen);
                    
                    
                    char value[valueLen + 1];
                    memset(value, 0, valueLen);
                    
                    memcpy(value, pLogArray, valueLen);
                    pLogArray += valueLen;
                    
                    std::string valueStr(value);
                    DebugLog("%s", valueStr.c_str());
                    kd->setValue(value);
                    
                    logInfo->data = kd;
                    
                    break;
                }
                case FLAG_KEY_VALUE_CHAINING_DATA:
                {
                    KeyValueChainingData* kcd = new KeyValueChainingData();
                    
                    uint8_t keyLen;
                    memcpy(&keyLen, pLogArray, sizeof(keyLen));
                    pLogArray += sizeof(keyLen);
                    
                    
                    char key[keyLen + 1];
                    memset(key, 0, keyLen);
                    
                    memcpy(key, pLogArray, keyLen);
                    pLogArray += keyLen;
                    
                    std::string keyStr(key);
                    DebugLog("%s", keyStr.c_str());
                    kcd->setKey(key);
                    
                    
                    uint8_t valueLen;
                    memcpy(&valueLen, pLogArray, sizeof(valueLen));
                    pLogArray += sizeof(valueLen);
                    
                    
                    char value[valueLen + 1];
                    memset(value, 0, valueLen);
                    
                    memcpy(value, pLogArray, valueLen);
                    pLogArray += valueLen;
                    
                    std::string valueStr(value);
                    DebugLog("%s", valueStr.c_str());
                    kcd->setValue(value);
                    
                    
                    int64_t chainingBA;
                    memcpy(&chainingBA, pLogArray, sizeof(chainingBA));
                    pLogArray += sizeof(chainingBA);
                    kcd->setIndBlockAddress(chainingBA);
                    
                    
                    logInfo->data = kcd;
                    
                    break;
                }
                    
                default:
                {
                    ErrorLog("invalid type - %d", dataType);
                    return false;
                }
            }
        }
        
        dequeue->push_back(logInfo);

        lastLogScn = logScn;
    }
    
    
    return true;
}

void LogFile::clear()
{
    logFileSize = sizeof(cln);
}

