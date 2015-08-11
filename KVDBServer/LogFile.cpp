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

LogFile::LogFile()
{
    
}

bool LogFile::initialize(const char* fileName)
{
    if ((fd = open(fileName, O_RDWR | O_SYNC)) < 0) // if not exist disk
    {
        if(createDisk(fileName) == false)
        {
            ErrorLog("create Disk fail");
            return false;
        }
    }
    
    logFileSize = lseek(fd, 0, SEEK_END);
    
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
    
    return true;
}

bool LogFile::writeLogFile(int logSize, const char* logArray)
{
    lseek(fd, 0, SEEK_END);
    
    if(write(fd, logArray, logSize) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    return true;
}

int LogFile::readLogFile(char** logArray) const
{
    int logFileSize = (int)lseek(fd, 0, SEEK_END);
    
    lseek(fd, 0, SEEK_SET);
    
    if(read(fd, *logArray, logFileSize) < 0)
    {
        ErrorLog("read block error");
        return -1;
    }
    return logFileSize;
}

void LogFile::clear()
{
    if(ftruncate(fd, 0) < 0)
    {
        ErrorLog("ftruncate error");
    }
    
    if(ftruncate(fd, MAX_LOG_FILE_SIZE) < 0)
    {
        ErrorLog("ftruncate error");
    }
}

