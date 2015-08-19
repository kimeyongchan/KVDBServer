//
//  LogBuffer.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__LogBuffer__
#define __KVDBServer__LogBuffer__

#include <string>

#include "Data.h"

#define IS_ALLOCATED_BLOCK      0b10000000
#define IS_FREE_BLOCK           0b01000000
#define IS_INSERT               0b00000001

#define MAX_LOG_BUFFER_SIZE     1024 * 1024 * 500

class LogBuffer
{
public:
    LogBuffer();
    ~LogBuffer();
    bool initialize(int _cln);
    bool saveLog(bool isAllocateBlock, bool isFreeBlock, bool isInsert, int64_t blockAddress, uint16_t freeSpace, uint16_t offsetLocation, uint16_t offset, const Data* data, int64_t prevBlockAddress = NULL, int64_t nextBlockAddress = NULL); // not allocate data
    int commitLogBuffer(char** pLogBuffer);
    //void clear();
    
private:
    int logLen;
    char* logBuffer;
    char* currentLogBufferSeek;
    int cln;
};

#endif /* defined(__KVDBServer__LogBuffer__) */
