#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include <vector>
#include "WorkerThread.h"
#include <map>

class RequestInfo;
class InsertRequestInfo;
class InsertDirectoryRequestInfo;
class FindRequestInfo;
class DeleteRequestInfo;
class Block;
class Data;
class NamedData;


class NamedCacheData
{
public:
    int       componentIdx;
    uint64_t  indirectionBlockAdr;
    
    NamedCacheData(int componentIdx, uint64_t indirectionBlockAdr)
    {
        this->componentIdx          = componentIdx;
        this->indirectionBlockAdr   = indirectionBlockAdr;
    }
};

/*
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
 */

class DirtyBlockInfo
{
public:
    Block*      block;
    bool        isAllocateBlock;
    bool        isFreeBlock;
    bool        isInsert;
    uint64_t    blockAddress;
    uint16_t    indirectionNum;
    uint64_t    prevBlockAddress;
    uint64_t    nextBlockAddress;
    bool        isLoging;
    
    DirtyBlockInfo(Block* dirtyBlock, bool isAlloc, bool isFree, bool insertState,
                   uint64_t blockAdr, uint16_t indNum, uint64_t prevBlockAdr = 0, uint64_t nextBlockAdr = 0, bool logingState = true)
    {
        block               = dirtyBlock;
        isAllocateBlock     = isAlloc;
        isFreeBlock         = isFree;
        isInsert            = insertState;
        blockAddress        = blockAdr;
        indirectionNum      = indNum;
        prevBlockAddress    = prevBlockAdr;
        nextBlockAddress    = nextBlockAdr;
        isLoging            = logingState;  // 블럭에 체이닝 주소만 바뀌었을때 : 로깅 안해도됨, 디스크에는 써야 함
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


class IOManager : public WorkerThread
{
public:
    IOManager();
    ~IOManager();

    ////////////virtual method///////////////
    void connected(const ConnectInfo* connectInfo);
    void disconnected(const ConnectInfo* connectInfo);
    void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
    
    ////////////////////////////////////////
    void receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void receiveMasterData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    bool parsingQuery(const char* query, int queryLen, RequestInfo** pri);

public:
    int8_t processInsert(InsertRequestInfo* reqInfo);
    int8_t processInsert(InsertDirectoryRequestInfo* reqInfo);
    void processFind(FindRequestInfo* reqInfo);
    void processDelete(DeleteRequestInfo* reqInfo);
    
private:
    
    std::vector<std::string>    componentList;
    std::vector<NamedCacheData> namedCacheDataList;
    std::map<uint64_t, Block*>  insertBufferCacheDataMap; //<블럭주소, 블럭>
    std::vector<uint64_t>       lastBlockChainingAdrList;
    
    
    std::vector<std::string> split(const std::string &s, char delim);
    
    IoMgrReturnValue checkBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, long lastIdx);
    IoMgrReturnValue findBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, long lastIdx);
    
    uint16_t ibaToOffsetIdx(uint64_t iba, uint64_t ba);
    uint64_t ibaToBa(uint64_t iba);
    bool compaction(Block* block);
    bool caching(NamedData* firstParentData);
    
    
    
public:
    
    void TEST_INSERT();
    void TEST_INSERT_DIR();
    void TEST_FIND();
    void TEST_DELETE();
    
    
};

#endif // __I_O_MANAGER_H__