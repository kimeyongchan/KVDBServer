//
//  LogFile.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__LogFile__
#define __KVDBServer__LogFile__


class LogFile
{
public:
    LogFile();
    
    bool initialize(const char* fileName);
    
    bool writeLogFile(const char* LogArrary);
    const char* readLogFile();
    void clear();
    
private:
    bool createDisk(const char* fileName);
    
private:
    int fd;
};

#endif /* defined(__KVDBServer__LogFile__) */
