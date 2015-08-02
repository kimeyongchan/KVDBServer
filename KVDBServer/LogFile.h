//
//  LogFile.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__LogFile__
#define __KVDBServer__LogFile__

class RequestInfo;


class LogFile
{
public:
    LogFile();
    
    bool initialize();
    
    bool writeLogFile(RequestInfo* reqInfo);
    bool readLogFile(RequestInfo* reqInfo);
};

#endif /* defined(__KVDBServer__LogFile__) */
