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

#define IS_ALLOCATED_NEW_BLOCK  0b10000000
#define IS_INSERT               0b00000001

#define MAX_LOG_BUFFER_SIZE     1024 * 1024 * 500

class LogBuffer
{
public:
    LogBuffer();
    ~LogBuffer();
    bool initialize();
    bool saveLog(bool isAllocateBlock, bool isInsert, int64_t IndBlockAddress, uint16_t offset, const Data* data); // not allocate data
    int readLogBuffer(const char** logBuffer) const;
    void clear();
    
private:
    int logLen;
    char* logBuffer;
    char* currentLogBufferSeek;
};

#endif /* defined(__KVDBServer__LogBuffer__) */
