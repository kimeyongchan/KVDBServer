#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "Data.h"
#include <map>

class Block
{
    uint16_t  indirectionCnt;  // 0~65535개 , 인다이렉션 개수
    uint16_t  freeSpace = 8* 1024;  //  블럭 잔여량  (8k = 8192byte)
    int64_t   chainingAddress;  // 블럭 체이닝 주소
    
    std::map<int16_t, Data*> indirectionDataMap; //  <offset, data>
    
    /*
    bool insertData(Data* data, unsigned long offset )
    {
        if(data == null)
            return false;
        
        unsigned short dataSize = data.getDataSize();
        
        if(remaining < dataSize)
            return false;
        
        dataList.insert(data);
        remaining   -= dataSize;
        
        indirectionList.insert(offset);
        
        return true;
    }
    
    Data* getData(unsigned short offSet)
    {
        int dataListSize = dataList.size();
        for(int i=0; i < dataListSize; ++i)
        {
            Data* data =dataList.get(i);
            
        }
    }
    */
};

#endif // __BLOCK_H__