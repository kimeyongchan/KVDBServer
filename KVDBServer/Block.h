#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "Defines.h"
#include <map>

#define BLOCK_FIRST_FREE_SPACE  BLOCK_SIZE - (sizeof(indirectionCnt)+ sizeof(freeSpace) + sizeof(chainingAddress))

class Data;

class Block
{
private:
    uint16_t  indirectionCnt;   // 0~65535개 , 인다이렉션 개수
    uint16_t  freeSpace ;       //  블럭 잔여량  (8k = 8192byte)
    int64_t   chainingAddress;  // 블럭 체이닝 주소
    
    std::map<int16_t, Data*> indirectionDataMap; //  <offset, data>
    
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
    
    const std::map<int16_t, Data*>* getIndirectionDataMap() const
    {
        return &indirectionDataMap;
    }
    
    bool insertData(int16_t offset, Data* data);
    bool deleteData(int16_t offset);
    Data* getData(int16_t offset);
};

#endif // __BLOCK_H__