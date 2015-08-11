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

class LogFile
{
public:
    LogFile();
    
    bool initialize(const char* fileName);
    
    bool writeLogFile(int logSize, const char* logArrary);
    int readLogFile(char** logArray) const;
    void clear();
    
private:
    bool createDisk(const char* fileName);
    
private:
    int fd;
    long logFileSize;
};

#endif /* defined(__KVDBServer__LogFile__) */
