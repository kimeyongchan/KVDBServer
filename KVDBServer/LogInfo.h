//
//  LogInfo.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 16..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef KVDBServer_LogInfo_h
#define KVDBServer_LogInfo_h

#include <stdint.h>
#include "Data.h"

class LogInfo
{
public:
    bool isAllocateBlock;
    bool isFreeBlock;
    bool isInsert;
    int64_t prevBlockAddress;
    int64_t blockAddress;
    uint16_t offsetLocation;
    uint16_t offset;
    Data* data;
};

#endif
