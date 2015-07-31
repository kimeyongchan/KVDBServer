#ifndef __KVDBServer__DiskManager__
#define __KVDBServer__DiskManager__

#include <cstdInt>

class SuperBlock;
class Block;

class DiskManager
{

public:
    DiskManager();
    ~DiskManager();
    
    bool initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize);
    int64_t requestNewBlock();
    bool findBlock(int64_t blockAddress, Block* block);
    bool writeBlock(uint64_t blockAddress, const Block* block);
    Block* readBlock(uint64_t blockAddress);
    
private:
    bool createDisk(const char* fileName, uint16_t blockSize, uint64_t diskSize);
    int bitToByte(uint64_t bitCount);
    
private:
    int fd;
    SuperBlock* superBlock;
    uint64_t seekLocation;
    
};

#endif // __KVDBServer__DiskManager__
