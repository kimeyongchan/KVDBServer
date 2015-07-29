#ifndef __BLOCK_H__
#define __BLOCK_H__


#include <map>

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
        freeSpace = 8192 - (sizeof(indirectionCnt)+ sizeof(freeSpace) + sizeof(chainingAddress));
    }
    
    
    uint16_t getIndirectionCnt()
    {
        return indirectionCnt;
    }
    
    uint16_t getFreeSpace()
    {
        return freeSpace;
    }
    
    
    void setChainingAddress(int64_t address)
    {
        chainingAddress = address;
    }
    
    int64_t getChaingAddress()
    {
        return chainingAddress;
    }
    
    
    bool insertData(int16_t offset, Data* data);
    bool deleteData(int16_t offset);
    Data* getData(int16_t offset);
    
    

};

#endif // __BLOCK_H__