#ifndef __BLOCK_H__
#define __BLOCK_H__


#include <map>

#define BLOCK_FIRST_FREE_SPACE 8192 - (sizeof(indirectionCnt)+ sizeof(freeSpace) + sizeof(chainingAddress))

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
    int64_t   chainingAddress;  // 블럭 체이닝 주소
    
    std::map<uint16_t, IndirectionData*> indirectionDataMap; //  <인디렉션넘버, indirectionData>
    
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
        indirectionCnt = 0;
        freeSpace = BLOCK_FIRST_FREE_SPACE;
        chainingAddress = NULL;
        indirectionDataMap.clear();
    }
    
    
    uint16_t getIndirectionCnt() const
    {
        return indirectionCnt;
    }
    
    void calculateFreeSpace(uint16_t dataSize, uint8_t offsetSize, bool isIncrease)
    {
        if(isIncrease == true)
            freeSpace +=(dataSize + offsetSize);
        else
            freeSpace -=(dataSize + offsetSize);
    }
    
    uint16_t getFreeSpace() const
    {
        return freeSpace;
    }
    
    
    void setChainingAddress(int64_t address)
    {
        chainingAddress = address;
    }
    
    int64_t getChaingAddress() const
    {
        return chainingAddress;
    }
    
    bool checkOffset(uint16_t offset)
    {
        for(auto iter:indirectionDataMap)
            if( iter.second->offset == offset)
                return true;
        
        return false;
    }
    
    bool insertData(int16_t offset, Data* data);
    bool deleteData(int16_t offset);
    Data* getData(int16_t offset);
    
    const std::map<uint16_t, IndirectionData*> getIndirectionDataMap() const
    {
        return indirectionDataMap;
    }
    
    

};

#endif // __BLOCK_H__