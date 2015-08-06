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

class RequestInfo;

class LogBuffer
{
public:
    LogBuffer();
    bool initialize();
    bool saveLog(bool isAllocateBlock, bool isInsert, int64_t IndBlockAddress, uint16_t offset, const Data* data); // not allocate data
    const char* readLogBuffer();
    void clear();
    
private:
    const char* DataToArray(const Data* data);
    
private:
    std::string logbufferStorage;
};

#endif /* defined(__KVDBServer__LogBuffer__) */
