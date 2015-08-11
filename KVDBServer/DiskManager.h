#ifndef __KVDBServer__DiskManager__
#define __KVDBServer__DiskManager__

#include <cstdInt>

class SuperBlock;
class Block;

const int BLOCK_HEADER_SIZE = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(int64_t);

class DiskManager
{
public:
    DiskManager();
    ~DiskManager();
    
    bool initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize, SuperBlock* superblock);
    int64_t requestNewBlock();
    bool writeBlock(uint64_t blockAddress, const Block* block);
    bool readBlock(uint64_t blockAddress, Block* block);
    
private:
    bool createDisk(const char* fileName, uint16_t blockSize, uint64_t diskSize);
    int bitToByte(uint64_t bitCount);
    bool writeDisk(uint64_t address, const void* buffer, int bufferSize);
    bool readDisk(uint64_t address, void* buffer, int readSize);
    
private:
    int fd;

    
};

#endif // __KVDBServer__DiskManager__
