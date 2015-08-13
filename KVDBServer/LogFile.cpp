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
    char filePath[1000] = {0, };
    memcpy(filePath, __FILE__, strlen(__FILE__));
    int slushCount = 0;
    for(int i = (int)strlen(filePath); i > 0; i--)
    {
        if(filePath[i] == '/')
        {
            slushCount++;
            if(slushCount == 2)
            {
                memcpy(filePath + i + 1, fileName, strlen(fileName));
                filePath[i+1+strlen(fileName)] = '\0';
                break;
            }
        }
    }
        
    if ((fd = open(filePath, O_RDWR | O_SYNC)) < 0) // if not exist disk
    {
        if(createDisk(filePath) == false)
        {
            ErrorLog("create Disk fail");
            return false;
        }
    }
    else
    {
        lseek(fd, 0, SEEK_SET);
        
        if(read(fd, &cln, sizeof(cln)) < 0)
        {
            ErrorLog("read block error");
            return false;
        }
        
        logFileSize = sizeof(cln);
    }
    
    
    
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
    
    lseek(fd, 0, SEEK_SET);
    
    cln = 0;
    
    if(write(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    logFileSize = sizeof(cln);
    
    return true;
}

bool LogFile::writeLogFile(int logSize, const char* logArray)
{
    lseek(fd, logFileSize, SEEK_SET);
    
    if(write(fd, logArray, logSize) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    logFileSize += logSize;
    
    cln++;
    
    lseek(fd, 0, SEEK_SET);
    
    if(write(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write cln error");
        return false;
    }

    return true;
}

long LogFile::recoveryLogFile(int diskCln, char** logArray)
{
    lseek(fd, sizeof(cln), SEEK_SET);
    
    if(read(fd, *logArray, logFileSize - sizeof(cln)) < 0)
    {
        ErrorLog("read block error");
        return -1;
    }
    
    long sendLogFileSize = logFileSize - sizeof(cln);
    
    logFileSize = sizeof(cln);
    
    return sendLogFileSize;
}

void LogFile::clear()
{
    logFileSize = sizeof(cln);
}

