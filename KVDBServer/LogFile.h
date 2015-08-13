//
//  LogFile.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__LogFile__
#define __KVDBServer__LogFile__

#define MAX_LOG_FILE_SIZE   1024 * 1024 * 500

#include <stdInt.h>

class LogFile
{
public:
    LogFile();
    
    bool initialize(const char* fileName);
    
    bool writeLogFile(int logSize, const char* logArrary);
    long recoveryLogFile(int diskCln, char** logArray);
    void clear();
    
    int getCln() { return cln; }
//    long getLogInfoByCln(int _cln); //ToDo. make logInfo class
    
private:
    bool createDisk(const char* fileName);
    
private:
    int fd;
    long logFileSize;
    uint32_t cln;
};

#endif /* defined(__KVDBServer__LogFile__) */
