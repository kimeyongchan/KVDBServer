//
//  Disk.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "Disk.h"

#include <fcntl.h>
#include <string.h>

#include "Defines.h"
#include "Block.h"
#include "Log.h"

bool Disk::initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize)
{
    int fd;
    
    const int ONE_KBYTE = 1024;
    
    char forMemsetArray[ONE_KBYTE];
    
    if ((fd = open(fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC, 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file create error");
        return false;
    }
    
    lseek(fd, 0, SEEK_SET);
    
    
    
    /////////////////////////// set block size
    
    if(write(fd, (void*)&blockSize, sizeof(blockSize)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    
    
    /////////////////////////// set block count
    
    uint64_t blockCount = ((diskSize - sizeof(int8_t) - sizeof(uint64_t) - sizeof(int64_t)) / (blockSize + (double)1/8)); // max block count check
    
    if(write(fd, &blockCount, sizeof(blockCount)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    
    
    /////////////////////////// bitArray memset 0
    
    int byteCntForUsingBlockBit = (int)(blockCount / 8);
    
    if(blockCount % 8 != 0)
    {
        byteCntForUsingBlockBit++;
    }
    
    char byteCnt[byteCntForUsingBlockBit];
    memset(byteCnt, 0, byteCntForUsingBlockBit);
    
    write(fd, byteCnt, byteCntForUsingBlockBit);

    
    
    //////////////////////////// set root address
    
    int64_t rootBlockAddress = lseek(fd, 0, SEEK_CUR) + sizeof(int64_t);
    
    write(fd, &rootBlockAddress, sizeof(rootBlockAddress));
    
    
    
    //////////////////////////////// caching superBlock
    
    superBlock.setBlockSize(blockSize);
    superBlock.setBlockCount(blockCount);
    superBlock.setRootBlockAddress(rootBlockAddress);
    superBlock.allocateUsingBlockBitArray(byteCntForUsingBlockBit);
    
    
    /////////////////////////// set root block
    
    Block rootBlock;
    
    char blockArray[BLOCK_SIZE];
    memset(blockArray, 0, BLOCK_SIZE);
    
    uint16_t indCnt = rootBlock.getIndirectionCnt();
    uint16_t freeSpace = rootBlock.getFreeSpace();
    int64_t chainingAddress = rootBlock.getChaingAddress();
    
    char* pblockArray = blockArray;
    
    memcpy(pblockArray, &indCnt, sizeof(indCnt));
    pblockArray += sizeof(indCnt);
    
    memcpy(pblockArray, &freeSpace, sizeof(freeSpace));
    pblockArray += sizeof(freeSpace);
    
    memcpy(pblockArray, &chainingAddress, sizeof(chainingAddress));
    pblockArray += sizeof(chainingAddress);
    
    
    lseek(fd, blockAddress, SEEK_SET);
    if(write(fd, blockArray, BLOCK_SIZE) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    return true;
    
    
    return true;

}


int Disk::openDisk()
{
    
}


int Disk::readDisk(int fd, void* buf, size_t bufSize)
{
    
}


int Disk::writeDisk(int fd, const void* buf, size_t bufSize)
{
    
}


void Disk::close()
{
    
}