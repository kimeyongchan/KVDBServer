#include "DiskManager.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Defines.h"
#include "SuperBlock.h"
#include "Block.h"
#include "Log.h"
#include "Data.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"
#include "LogInfo.h"


DiskManager::DiskManager()
{
    
}


DiskManager::~DiskManager()
{
    
}


bool DiskManager::initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize, SuperBlock* _superBlock) //if not exist disk
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
        if(createDisk(filePath, blockSize, diskSize) == false)
        {
            ErrorLog("create Disk fail");
            return false;
        }
    }
    
    lseek(fd, 0, SEEK_SET);
    
    superBlock = new SuperBlock();
    
    /////////////////////////// get block size
    
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
    _superBlock->setBlockSize(blockSize);
    
    
    
    
    /////////////////////////// get block count
    
    uint64_t blockCount;
    
    if(read(fd, &blockCount, sizeof(blockCount)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    superBlock->setBlockCount(blockCount);
    _superBlock->setBlockCount(blockCount);
    
    
    /////////////////////////// get cln
    
    
    uint32_t cln;
    
    if(read(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write error");
        return false;
    }
    
    superBlock->setCln(cln);
    _superBlock->setCln(cln);
    
    
    /////////////////////////// bitArray memset 0
    
    int byteCntForUsingBlockBit = bitToByte(blockCount);
    
    superBlock->allocateUsingBlockBitArray(byteCntForUsingBlockBit);
    _superBlock->allocateUsingBlockBitArray(byteCntForUsingBlockBit);

    if(read(fd, superBlock->getUsingBlockBitArray(), byteCntForUsingBlockBit) < 0)
    {
        ErrorLog("read Error");
        return false;
    }
    
    memcpy(_superBlock->getUsingBlockBitArray(), superBlock->getUsingBlockBitArray(), byteCntForUsingBlockBit);
    
    //////////////////////////// get root address
    
    int64_t rootBlockAddress;

    if(read(fd, &rootBlockAddress, sizeof(rootBlockAddress)) < 0)
    {
        ErrorLog("read Error");
        return false;
    }
    
    superBlock->setRootBlockAddress(rootBlockAddress);
    _superBlock->setRootBlockAddress(rootBlockAddress);
    
    
    ////////////////////////// get root
    
    Block* rootBlock = new Block();
    
    if(readBlock(rootBlockAddress, rootBlock) == false)
    {
        ErrorLog("root block read error");
        return false;
    }
    
    superBlock->setRootBlock(rootBlock);
    
    
    Block* _rootBlock = new Block();
    if(readBlock(rootBlockAddress, _rootBlock) == false)
    {
        ErrorLog("root block read error");
        return false;
    }
    
    _superBlock->setRootBlock(_rootBlock);

    

    return true;
}


bool DiskManager::createDisk(const char* fileName, uint16_t blockSize, uint64_t diskSize)
{
    const int ONE_KBYTE = 1024;
    
    char forMemsetArray[ONE_KBYTE];
    
    memset(forMemsetArray, 0, sizeof(ONE_KBYTE));
    
    if ((fd = open( fileName, O_RDWR | O_CREAT | O_EXCL | O_SYNC , 0666)) < 0) // ToDo. correct permission later
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
    
    
    /////////////////////////// set cln
    
    uint32_t cln = 0;
    
    if(write(fd, &cln, sizeof(cln)) < 0)
    {
        ErrorLog("write cln error");
        return false;
    }
    
    /////////////////////////// bitArray memset 0
    
    int byteCntForUsingBlockBit = (int)(blockCount / 8);
    
    if(blockCount % 8 != 0)
    {
        byteCntForUsingBlockBit++;
    }
    
    DebugLog("byteCntForUsingBlockBit : %d", byteCntForUsingBlockBit);
    
    uint8_t rootSetByte = 0x80; // 1000000 bit
    
    write(fd, &rootSetByte, 1);
    
    byteCntForUsingBlockBit -= 1;
    
    for(; byteCntForUsingBlockBit >= ONE_KBYTE; byteCntForUsingBlockBit -= ONE_KBYTE)
    {
        write(fd, forMemsetArray, ONE_KBYTE);
    }
    
    write(fd, forMemsetArray, byteCntForUsingBlockBit);

    
    //////////////////////////// set root address
    
    int64_t rootBlockAddress = lseek(fd, 0, SEEK_CUR) + sizeof(int64_t);
    
    write(fd, &rootBlockAddress, sizeof(rootBlockAddress));
    
    
    
    /////////////////////////// set root block
    
    Block rootBlock;
    
    if(writeBlock(rootBlockAddress, &rootBlock) == false)
    {
        ErrorLog("writeBlock error");
        return false;
    }
    
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


bool DiskManager::readBlock(uint64_t blockAddress, Block *block)
{
    char blockArray[BLOCK_SIZE];
    memset(blockArray, 0, BLOCK_SIZE);
    
    if(readDisk(blockAddress, blockArray, BLOCK_SIZE) == false)
    {
        ErrorLog("readDisk error");
        return false;
    }
    
    char* pBlockArray = blockArray;
    
    uint16_t indCnt;
    memcpy(&indCnt, pBlockArray, sizeof(indCnt));
    pBlockArray += sizeof(indCnt);
    block->setIndirectionCnt(indCnt);
    
    uint16_t freeSpace;
    memcpy(&freeSpace, pBlockArray, sizeof(freeSpace));
    pBlockArray += sizeof(freeSpace);
    block->setFreeSpace(freeSpace);
    
    int64_t nextBlockAddress;
    memcpy(&nextBlockAddress, pBlockArray, sizeof(nextBlockAddress));
    pBlockArray += sizeof(nextBlockAddress);
    block->setChainingAddress(nextBlockAddress);
    
    
    uint16_t dataAddress = 0;
    int countDiskInd = 0;
    int i = 0;
    for(; countDiskInd < indCnt; i++)
    {
        dataAddress = 0;
        memcpy(&dataAddress, pBlockArray, sizeof(dataAddress));
        pBlockArray += sizeof(dataAddress);
        
        if(dataAddress == 0)
        {
            continue;
        }
        else
        {
            int8_t formatType;
            char* dataBlockArray = blockArray + dataAddress;
            memcpy(&formatType, dataBlockArray, sizeof(formatType));
            dataBlockArray += sizeof(formatType);
            
            uint8_t keyLen;
            memcpy(&keyLen, dataBlockArray, sizeof(keyLen));
            dataBlockArray += sizeof(keyLen);
            
            char key[keyLen + 1];
            memset(key + keyLen, 0, 1);
            memcpy(key, dataBlockArray, keyLen);
            dataBlockArray += keyLen;
            
            std::string keyString(key);
            
            Data* data = NULL;
            
            switch (formatType)
            {
                case 0: //directory
                {
                    DirectoryData* dd = new DirectoryData();

                    int64_t indBlockAddress; // = dd->getIndBlockAddress();
                    memcpy(&indBlockAddress, dataBlockArray, sizeof(indBlockAddress));
                    dd->setIndBlockAddress(indBlockAddress);
                    data = dd;
                    break;
                }
                case 1: //not chaining kv data
                {
                    KeyValueData* kd = new KeyValueData();

                    
                    uint32_t valueLen;// = (uint32_t)data.length();
                    
                    memcpy(&valueLen, dataBlockArray, sizeof(valueLen));
                    dataBlockArray += sizeof(valueLen);
                    char value[valueLen + 1];
                    memset(value + valueLen, 0, 1);
                    memcpy(value, dataBlockArray, valueLen);
                    dataBlockArray += valueLen;
                    
                    std::string valueString(value); // = kd->getValue();
                    
                    kd->setValue(valueString);
                    
                    data = kd;
                    
                    break;
                }
                case 2:
                {
                    KeyValueChainingData* kcd = new KeyValueChainingData();
                    
                    
                    uint32_t valueLen;
                    
                    memcpy(&valueLen, dataBlockArray, sizeof(valueLen));
                    dataBlockArray += sizeof(valueLen);
                    char value[valueLen + 1];
                    memset(value + valueLen, 0, 1);
                    memcpy(value, dataBlockArray, valueLen);
                    dataBlockArray += valueLen;
                    
                    std::string valueString(value); // = kd->getValue();
                    
                    kcd->setValue(valueString);
                    
                    int64_t chainingAddress;
                    memcpy(&chainingAddress, dataBlockArray, sizeof(chainingAddress));
                    dataBlockArray += sizeof(chainingAddress);
                    
                    kcd->setIndBlockAddress(chainingAddress);
                    
                    data = kcd;
                    
                    break;
                }
                default:
                    ErrorLog("invalid format type - %d", formatType);
                    return false;
            }
            
            data->setFormatType(formatType);
            data->setKey(keyString);
            
            if(block->insertData(i, dataAddress, data) == false)
            {
                ErrorLog("insertData error");
                return false;
            }
            
            countDiskInd++;
        }
    }
    
    
    return true;
}

bool DiskManager::writeBlock(uint64_t blockAddress, const Block *block)
{
    char blockArray[BLOCK_SIZE];
    memset(blockArray, 0, BLOCK_SIZE);
    
    uint16_t indCnt = block->getIndirectionCnt();
    uint16_t freeSpace = block->getFreeSpace();
    int64_t nextBlockAddress = block->getChaingAddress();
    
    char* pblockArray = blockArray;
    
    memcpy(pblockArray, &indCnt, sizeof(indCnt));
    pblockArray += sizeof(indCnt);
    
    memcpy(pblockArray, &freeSpace, sizeof(freeSpace));
    pblockArray += sizeof(freeSpace);
    
    memcpy(pblockArray, &nextBlockAddress, sizeof(nextBlockAddress));
    pblockArray += sizeof(nextBlockAddress);
    
    const std::map<uint16_t, IndirectionData*>* map = block->getIndirectionDataMap();
    
    std::map<uint16_t, IndirectionData*>::const_iterator itr;
    
    for(itr = map->begin(); itr != map->end(); itr++)
    {
        uint16_t key = itr->first;
        IndirectionData* indData = itr->second;
        
        uint16_t offset = indData->offset;
        
        memcpy(pblockArray + (key * sizeof(uint16_t)), &offset, sizeof(offset));

        
        int8_t formatType = indData->data->getFormatType();
        memcpy(blockArray + offset, &formatType, sizeof(formatType));
        
        
        uint8_t keyLen = indData->data->getKeyLength();
        memcpy(blockArray + offset + sizeof(formatType), &keyLen, sizeof(keyLen));
        memcpy(blockArray + offset + sizeof(formatType) + sizeof(keyLen), indData->data->getKey().c_str(), keyLen);
        
        char* formatArray = blockArray + offset + sizeof(formatType) + sizeof(keyLen)+ keyLen;
        
        switch (formatType)
        {
            case 0: //directory
            {
                DirectoryData* dd = (DirectoryData*)indData->data;
                
                int64_t indBlockAddress = dd->getIndBlockAddress();
                memcpy(formatArray, &indBlockAddress, sizeof(indBlockAddress));
                break;
            }
            case 1: //not chaining kv data
            {
                KeyValueData* kd = (KeyValueData*)indData->data;
                
                std::string data = kd->getValue();
                uint32_t dataLen = (uint32_t)data.length();
                
                memcpy(formatArray, &dataLen, sizeof(dataLen));
                memcpy(formatArray + sizeof(dataLen), data.c_str(), dataLen);
                break;
            }
            case 2:
            {
                KeyValueChainingData* kd = (KeyValueChainingData*)indData->data;
                
                std::string data = kd->getValue();
                uint32_t dataLen = (uint32_t)data.length();
                int64_t chainingAddress = kd->getIndBlockAddress();
                
                memcpy(formatArray, &dataLen, sizeof(dataLen));
                memcpy(formatArray + sizeof(dataLen), data.c_str(), dataLen);
                memcpy(formatArray + sizeof(dataLen) + dataLen, &chainingAddress, sizeof(chainingAddress));
                break;
            }
            default:
                ErrorLog("invalid format type - %d", formatType);
                break;
        }
        
        
        
    } // end of for
    
    if(writeDisk(blockAddress, blockArray, BLOCK_SIZE) == false)
    {
        ErrorLog("write block error");
        return false;
    }
    
    return true;
}

bool DiskManager::writeDisk(uint64_t address, const void* buffer, int bufferSize)
{
    lseek(fd, address, SEEK_SET);
    
    if(write(fd, buffer, bufferSize) < 0)
    {
        ErrorLog("write block error");
        return false;
    }
    
    return true;
}


bool DiskManager::readDisk(uint64_t address, void* buffer, int readSize)
{
    lseek(fd, address, SEEK_SET);
    
    if(read(fd, buffer, readSize) < 0)
    {
        ErrorLog("read block error");
        return false;
    }
    
    return true;
}

bool DiskManager::recovery(const LogInfo* logInfo)
{
    bool isAllocateBlock = logInfo->isAllocateBlock;
    bool isFreeBlock = logInfo->isFreeBlock;
    bool isInsert = logInfo->isInsert;
    
    int64_t prevBlockAddress = logInfo->prevBlockAddress;
    int64_t blockAddress = logInfo->blockAddress;
    uint16_t offsetLocation = logInfo->offsetLocation;
    uint16_t offset = logInfo->offset;
    
    
    
    if(isAllocateBlock)
    {
        int bitSeek = convertAddressToBitSeek(blockAddress);
        
        int bitIndex = (bitSeek / 8);
        int bitPosition = bitSeek % 8;
        
        char bit = 1;
        bit = bit << bitPosition;
        
        char* bitArray = superBlock->getUsingBlockBitArray();
        
        char* pBit = bitArray + bitIndex;
        
        *pBit |= bit;
        
        
    }
    
    return true;
}

int DiskManager::convertAddressToBitSeek(int64_t blockAddress)
{
    return (int)((blockAddress - superBlock->getRootBlockAddress()) / superBlock->getBlockSize());
}


bool DiskManager::writeBitArray(const char* bitArray)
{
    uint64_t bitArrayAddress = sizeof(uint16_t) + sizeof(uint64_t) + sizeof(uint32_t);
    
    int byteArrayCount = (int)(superBlock->getBlockCount() / 8);
    
    if(superBlock->getBlockCount() % 8 != 0 )
        byteArrayCount++;
    
    writeDisk(bitArrayAddress, bitArray, byteArrayCount);
    
    return true;
}