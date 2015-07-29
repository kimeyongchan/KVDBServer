#include "Block.h"
#include "Data.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"

bool Block::insertData(int16_t offset, Data* data)
{
    if(data == NULL)
        return false;
    
    uint16_t dataSize = data->getDataSize();
  

    if(freeSpace < dataSize)
        return false;
    
    if(indirectionDataMap.find(offset) != indirectionDataMap.end())  // insert할 키 이미 있으면 실제 값 리턴 없으면 end() 리턴
        return false;
    
    
    indirectionDataMap.insert(std::pair<int16_t, Data*>(offset,data));
    freeSpace   -= dataSize;
    increaseIndirectionCnt();
    
    return true;
}

bool Block::deleteData(int16_t offset)
{
    auto iter = indirectionDataMap.find(offset);
    
    if(iter == indirectionDataMap.end())
        return false; //찾는 키 없다.
    
    
    Data* data = iter->second;
    uint16_t dataSize = data->getDataSize();
    
    
    indirectionDataMap.erase(iter);
    freeSpace   += dataSize;
    decreaseIndirctionCnt();
    
    return true;
    
}

Data* Block::getData(int16_t offset)
{
    auto iter = indirectionDataMap.find(offset);
    
    if(iter == indirectionDataMap.end())
        return NULL; //찾는 데이터
    
    
    return iter->second;
}




