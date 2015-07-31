#include "DiskManager.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "SuperBlock.h"
#include "Block.h"
#include "Log.h"


DiskManager::DiskManager()
{
    
}


DiskManager::~DiskManager()
{
    
}


bool DiskManager::initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize) //if not exist disk
{
    const int ONE_KBYTE = 1024;
    
    char forMemsetArray[ONE_KBYTE];
    
    if ((fd = open(fileName, O_RDWR | O_SYNC)) < 0) // if not exist disk
    {
        if(createDisk(fileName, blockSize, diskSize) == false)
        {
            ErrorLog("create Disk fail");
            return false;
        }
    }
    
    seekLocation = lseek(fd, 0, SEEK_SET);
    
    superBlock = new SuperBlock();
    
    /////////////////////////// set block size
    
    uint16_t diskBlockSize;
    
    if(read(fd, (void*)&diskBlockSize, sizeof(diskBlockSize)) < 0)
    {
        ErrorLog("read error");
        return false;
    }
    
    if(diskBlockSize != blockSize)
    {
        ErrorLog("not correct block size - disk : %d, memory : %d", diskBlockSize, blockSize);
        return false;
    }
    
    superBlock->setBlockSize(blockSize);
    
    DebugLog("seekLocation : %lld %d %d", seekLocation, blockSize, sizeof(blockSize));
    
    
    
    /////////////////////////// set block count
    
    uint64_t blockCount;
    
    if(read(fd, &blockCount, sizeof(blockCount)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    superBlock->setBlockCount(blockCount);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    
    
    /////////////////////////// bitArray memset 0
    
    int byteCntForUsingBlockBit = bitToByte(blockCount);
    
    DebugLog("byteCntForUsingBlockBit : %d", byteCntForUsingBlockBit);
    
    superBlock->allocateUsingBlockBitArray(byteCntForUsingBlockBit);

    if(read(fd, superBlock->getUsingBlockBitArray(), byteCntForUsingBlockBit) < 0)
    {
        ErrorLog("read Error");
        return false;
    }
    
    DebugLog("seekLocation : %lld", seekLocation);

    
    
    //////////////////////////// set root address
    
    int64_t rootBlockAddress;

    if(read(fd, &rootBlockAddress, sizeof(rootBlockAddress)) < 0)
    {
        ErrorLog("read Error");
        return false;
    }
    superBlock->setRootBlockAddress(rootBlockAddress);
    
    seekLocation = lseek(fd, 0, SEEK_CUR);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    return true;
}

bool DiskManager::createDisk(const char* fileName, uint16_t blockSize, uint64_t diskSize)
{
    const int ONE_KBYTE = 1024;
    
    char forMemsetArray[ONE_KBYTE];
    
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC , 0666)) < 0) // ToDo. correct permission later
    {
        ErrorLog("file create error");
        return false;
    }

    seekLocation = lseek(fd, 0, SEEK_SET);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    
    
    /////////////////////////// set block size
    
    if(write(fd, (void*)&blockSize, sizeof(blockSize)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    seekLocation = lseek(fd, 0, SEEK_CUR);
    
    DebugLog("seekLocation : %lld %d %d", seekLocation, blockSize, sizeof(blockSize));
    
    
    
    /////////////////////////// set block count
    
    uint64_t blockCount = ((diskSize - sizeof(int8_t) - sizeof(uint64_t) - sizeof(int64_t)) / (blockSize + (double)1/8)); // max block count check
    
    if(write(fd, &blockCount, sizeof(blockCount)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    seekLocation = lseek(fd, 0, SEEK_CUR);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    
    
    /////////////////////////// bitArray memset 0
    
    int byteCntForUsingBlockBit = (int)(blockCount / 8);
    
    if(blockCount % 8 != 0)
    {
        byteCntForUsingBlockBit++;
    }
    
    DebugLog("byteCntForUsingBlockBit : %d", byteCntForUsingBlockBit);
    
    for(; byteCntForUsingBlockBit >= ONE_KBYTE; byteCntForUsingBlockBit -= ONE_KBYTE)
    {
        write(fd, forMemsetArray, ONE_KBYTE);
    }
    
    write(fd, forMemsetArray, byteCntForUsingBlockBit);
    seekLocation = lseek(fd, 0, SEEK_CUR);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    //////////////////////////// set root address
    
    int64_t rootBlockAddress = seekLocation + sizeof(int64_t);
    
    write(fd, &rootBlockAddress, sizeof(rootBlockAddress));
    
    seekLocation = lseek(fd, 0, SEEK_CUR);
    
    DebugLog("seekLocation : %lld", seekLocation);
    
    
    
    /////////////////////////// set root block
    
    Block rootBlock;
    
    
    
    
    return true;
}

int DiskManager::bitToByte(uint64_t bitCount)
{
    int byteCnount = (int)(bitCount / 8);
    
    if(bitCount % 8 != 0)
    {
        byteCnount++;
    }
    
    return byteCnount;
}

int64_t DiskManager::requestNewBlock()
{
    return 0;
}


bool DiskManager::findBlock(int64_t blockAddress, Block* block)
{
    return false;
}

bool DiskManager::writeBlock(uint64_t blockAddress, const Block *block)
{
    uint16_t iba = 2332;
    
    char blockArray[BLOCK_SIZE];
    memset(blockArray, 0, BLOCK_SIZE);
    
    uint16_t indCnt = block->getIndirectionCnt();
    uint16_t freeSpace = block->getFreeSpace();
    int64_t chainingAddress = block->getChaingAddress();
    
    char* pblockArray = blockArray;
    
    memcpy(pblockArray, &indCnt, sizeof(indCnt));
    pblockArray += sizeof(indCnt);
    
    memcpy(pblockArray, &freeSpace, sizeof(freeSpace));
    pblockArray += sizeof(freeSpace);
    
    memcpy(pblockArray, &chainingAddress, sizeof(chainingAddress));
    pblockArray += sizeof(chainingAddress);
    
    const std::map<int16_t, Data*>* map = block->getIndirectionDataMap();
    
    std::map<int16_t, Data*>::const_iterator itr;
    
    for(itr = map->begin(); itr != map->end(); itr++)
    {
        uint16_t key = itr->first;
        Data* data = itr->second;
        
        
        memcpy(pblockArray + (key * sizeof(uint16_t)), &iba, sizeof(iba));
        //data write
        
    }
    
    lseek(fd, blockAddress, SEEK_SET);
    if(write(fd, blockArray, BLOCK_SIZE) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    return true;
}