#ifndef __SUPER_BLOCK_H__
#define __SUPER_BLOCK_H__

#include <cstdInt>
#include <cstdlib>

class Block;

class SuperBlock
{
private:
    uint16_t blockSize;
    uint64_t blockCount;
    char* usingBlockBitArray;
    int64_t rootBlockAddress;
    Block* rootBlock;

    bool isDirty;
    
public:
    
    ~SuperBlock()
    {
        delete usingBlockBitArray;
    }
    
    uint16_t getBlockSize()
    {
        return blockSize;
    }
    
    void setBlockSize(uint16_t blockSize)
    {
        this->blockSize = blockSize;
    }
    
    int64_t getBlockCount()
    {
        return blockCount;
    }
    
    void setBlockCount(int64_t blockCount)
    {
        this->blockCount = blockCount;
    }
    
    int64_t getRootBlockAddress()
    {
        return rootBlockAddress;
    }

    void setRootBlockAddress(int64_t rootBlockAddress)
    {
        this->rootBlockAddress = rootBlockAddress;
    }
    
    void allocateUsingBlockBitArray(int arrayLen)
    {
        usingBlockBitArray = (char*)malloc(sizeof(char) * arrayLen);
    }
    
    char* getUsingBlockBitArray()
    {
        return usingBlockBitArray;
    }
    
    void setRootBlock(Block* block)
    {
        rootBlock = block;
    }
    
    Block* getRootBlock()
    {
        return rootBlock;
    }
    
    
    void setDirty(bool dirtyState)
    {
        isDirty = dirtyState;
    }
    
    bool getDirty()
    {
        return isDirty;
    }
};

#endif // __SUPER_BLOCK_H__
