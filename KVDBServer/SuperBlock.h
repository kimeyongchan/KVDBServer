#ifndef __SUPER_BLOCK_H__
#define __SUPER_BLOCK_H__

#include <cstdInt>
#include <cstdlib>

class SuperBlock
{
private:
    int8_t blockSize;
    int64_t blockCount;
    int64_t rootBlockAddress;
    char* usingBlockBitArray;
    
public:
    void setBlockSize(int8_t blockSize)
    {
        this->blockSize = blockSize;
    }
    
    void setBlockCount(int64_t blockCount)
    {
        this->blockCount = blockCount;
    }
    
    void setRootBlockAddress(int64_t rootBlockAddress)
    {
        this->rootBlockAddress = rootBlockAddress;
    }
    
    void allocateUsingBlockBitArray(int arrayLen)
    {
        usingBlockBitArray = (char*)malloc(sizeof(char) * arrayLen);
    }
};

#endif // __SUPER_BLOCK_H__
