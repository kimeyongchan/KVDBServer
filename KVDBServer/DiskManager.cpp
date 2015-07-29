#include "DiskManager.h"

#include <fcntl.h>
#include <unistd.h>

#include "SuperBlock.h"
#include "Log.h"


DiskManager::DiskManager()
{
    
}


DiskManager::~DiskManager()
{
    
}
/*

bool DiskManager::initialize(const char* fileName, int8_t blockSize, int64_t blockCount) //if not exist file
{
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC , 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file open error");
        return false;
    }
    
    write
    
    superBlock = new SuperBlock();
    superBlock->se
    
    
    
    return true;
}

bool DiskManager::initialize(const char* fileName, int8_t blockSize)
{
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_SYNC , 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file open error");
        return false;
    }
    
    superBlock = new SuperBlock();
    superBlock->se
    
    
    
    return true;
}
*/