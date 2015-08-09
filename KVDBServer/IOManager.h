#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include <vector>
#include <map>

class InsertRequestInfo;
class InsertDirectoryRequestInfo;
class FindRequestInfo;
class DeleteRequestInfo;
class Block;
class Data;


class NamedCacheData
{
    int       componentIdx;
    uint64_t  indirectionBlockAdr;
    
    NamedCacheData(int componentIdx, uint64_t indirectionBlockAdr)
    {
        this->componentIdx          = componentIdx;
        this->indirectionBlockAdr   = indirectionBlockAdr;
    }
};

class LogBufferInsertData
{
public:
    bool        isAllocateBlock;
    bool        isInsert;
    uint64_t    indBlockAddress;
    uint16_t    offset;
    const Data* data;
    
    LogBufferInsertData(bool isAlloc, bool insertState, uint64_t indBlockAdr, uint16_t offset, const Data* d)
    {
        isAllocateBlock = isAlloc;
        isInsert = insertState;
        indBlockAddress = indBlockAdr;
        this->offset = offset;
        data = d;
    }
};


class FindReturnValue
{
public:
    const Block* block;
    int componentIdx;
    
    FindReturnValue(const Block* block, int componentIdx)
    {
        this->block = block;
        this->componentIdx = componentIdx;
    }
};

class IoMgrReturnValue
{
public:
    Block* block;
    int   componentIdx;
    int   returnCode;  // -1일때 루트 블럭
    
    IoMgrReturnValue(Block* block, int componentIdx, int returnCode)
    {
        this->block         = block;
        this->componentIdx  = componentIdx;
        this->returnCode    = returnCode;
    }
};

class IOManager
{
public:
    IOManager();
    ~IOManager();
    
public:
    int8_t processInsert(InsertRequestInfo* reqInfo);
    int8_t processInsert(InsertDirectoryRequestInfo* reqInfo);
    void processFind(FindRequestInfo* reqInfo);
    void processDelete(DeleteRequestInfo* reqInfo);
    
private:
    
    std::vector<std::string>    componentList;
    std::vector<NamedCacheData> namedCacheDataList;
    std::vector<uint64_t>       insertNamedDataIndirectionAdrList;
    std::map<uint64_t, Block*>  insertBufferCacheDataMap; //<블럭주소, 블럭>
    
    std::vector<std::string> split(const std::string &s, char delim);
    
    FindReturnValue checkBufferCacheAndDisk_2(uint64_t indirectionBa, int curIdx, int lastIdx);
    IoMgrReturnValue checkBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, int lastIdx);
    
    uint16_t ibaToOffsetIdx(uint64_t iba, uint64_t ba);
    uint64_t ibaToBa(uint64_t iba);
    bool compaction(Block* block);
    
};

#endif // __I_O_MANAGER_H__