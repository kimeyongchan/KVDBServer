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
    
    uint64_t location = lseek(fd, 0, SEEK_END);
    
    
    
    return true;
}


bool LogFile::createDisk(const char* fileName)
{
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC , 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file create error");
        return false;
    }
    
    return true;
}

bool LogFile::writeLogFile(const char* LogArrary)
{
    
    return true;
}

const char* LogFile::readLogFile()
{
    
    return NULL;
}

void LogFile::clear()
{
    
}