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

class RequestInfo;

class LogBuffer
{
public:
    LogBuffer();
    bool initialize();
    bool saveRequestInfo(const RequestInfo* reqInfo);
    const char* sendLogToLogFile();
    
private:
    std::string unParsing(const RequestInfo* reqInfo);
    
private:
    std::string logbufferStorage;
};

#endif /* defined(__KVDBServer__LogBuffer__) */
