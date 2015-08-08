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

    std::vector<uint64_t>       insertNamedDataIndirectionAdrList;
    std::map<uint64_t, Block*>  insertBufferCacheDataMap; //<블럭주소, 블럭>
    
    std::vector<std::string> split(const std::string &s, char delim);
    
    int8_t checkBufferCacheAndDisk_1(uint64_t indirectionBa, int curIdx, int lastIdx);
    uint16_t ibaToOffsetIdx(uint64_t iba, uint64_t ba);
  
    uint64_t ibaToBa(uint64_t iba);
    
};

#endif // __I_O_MANAGER_H__