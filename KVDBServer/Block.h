#ifndef __BLOCK_H__
#define __BLOCK_H__


#include <map>

#define BLOCK_INDIRECTION_COUNT_OFFSET  0
#define BLOCK_FREE_SPACE_OFFSET         sizeof(uint16_t)
#define BLOCK_CHAINING_OFFSET           sizeof(uint16_t) + sizeof(uint16_t)
#define BLOCK_HEADER_SIZE               sizeof(uint16_t) + sizeof(uint16_t) + sizeof(int64_t)
#define BLOCK_FIRST_FREE_SPACE 8192 - (sizeof(uint16_t)+ sizeof(uint16_t) + sizeof(uint64_t))

class Data;

class IndirectionData
{
public:
    IndirectionData(uint16_t offset, Data* data)
    {
        this->offset = offset;
        this->data   = data;
    }
    uint16_t offset;
    Data* data;
};

class Block
{

private:
    
    uint16_t  indirectionCnt;   // 0~65535개 , 인다이렉션 개수
    uint16_t  freeSpace ;       //  블럭 잔여량  (8k = 8192byte)
    uint64_t  chainingAddress;  // 블럭 체이닝 주소
    
    std::map<uint16_t, IndirectionData*> indirectionDataMap; //  <인디렉션넘버, indirectionData>
    bool isDirty;
 
    void increaseIndirectionCnt()
    {
        ++indirectionCnt;
    }
    
    void decreaseIndirctionCnt()
    {
        --indirectionCnt;
    }
    
public:
    Block()
    {
        init();
    }
    
    ~Block();
    
    static uint8_t getBlockHeaderSize()
    {
        return sizeof(indirectionCnt) + sizeof(freeSpace) + sizeof(chainingAddress);
    }
    
    void setIndirectionCnt(uint16_t indirectionCnt)
    {
        this->indirectionCnt = indirectionCnt;
    }
    void setFreeSpace(uint16_t freeSpace)
    {
        this->freeSpace = freeSpace;
    }
    
    uint16_t getIndirectionCnt() const
    {
        return indirectionCnt;
    }

    
    uint16_t getFreeSpace() const
    {
        return freeSpace;
    }
    
    
    void setChainingAddress(uint64_t address)
    {
        chainingAddress = address;
    }
    
    uint64_t getChaingAddress() const
    {
        return chainingAddress;
    }
    
    void setDirty(bool dirtyState)
    {
        isDirty = dirtyState;
    }
    
    bool getDirty()
    {
        return isDirty;
    }
    
    
    bool checkOffset(uint16_t offset)
    {
        for(auto iter:indirectionDataMap)
            if( iter.second->offset == offset)
                return true;
        
        return false;
    }
    
    bool insertData(uint16_t idx, uint16_t offset, Data* data);
    bool deleteData(uint16_t idx);
    bool deleteData(std::string dataKey);
    Data* getData(uint16_t idx);
    Data* getData(std::string dataKey);
    
    uint64_t getFirstIndirectionBlockAdr(uint64_t blockAdr)
    {
        return blockAdr + (sizeof(indirectionCnt)+ sizeof(freeSpace) + sizeof(chainingAddress));
    }
    
    uint64_t getIndirectionBlockAdr(uint64_t blockAdr, uint16_t indirectionNumber);
    uint16_t getNewOffset(uint16_t dataSize);
    uint16_t getLargestOffset(uint16_t limitValue);
    uint16_t getNewIndirectionNumber();
    int16_t getLastIndirectionNumber()
    {
        int16_t lastIndirectionNumber = -1;
        
        for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
        {
            if(iter->first > lastIndirectionNumber)
                lastIndirectionNumber = iter->first;
        }
        
        return lastIndirectionNumber;
    }
    
    uint16_t getIndirectionDataMapSize()
    {
        return indirectionDataMap.size();
    }
    
    void init()
    {
        chainingAddress = NULL;
        indirectionCnt = 0;
        freeSpace = BLOCK_FIRST_FREE_SPACE;
        indirectionDataMap.clear();
        isDirty = true;
    }
    
    uint16_t getIndNumByOffset(uint16_t offset);
    uint16_t getIndNumByKey(std::string componentKey);
    uint16_t getOffsetByIndNum(uint16_t indNum);
    uint16_t getOffsetByKey(std::string componentKey);
    uint16_t getLargestDatasDistanceSize();
    
    
    const std::map<uint16_t, IndirectionData*>* getIndirectionDataMap() const
    {
        return &indirectionDataMap;
    }
    
private:
    
    void calculateFreeSpace(uint16_t dataSize, uint8_t offsetSize, bool isInsertData)
    {
        if(isInsertData == false)
            freeSpace +=(dataSize + offsetSize);
        else
            freeSpace -=(dataSize + offsetSize);
    }
    

};

#endif // __BLOCK_H__