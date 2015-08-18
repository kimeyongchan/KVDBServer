#include "Block.h"
#include "Data.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"
#include "Defines.h"
#include <vector>


Block::~Block()
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
    {
        if(iter->second->data != NULL)
        {
            delete iter->second->data;
            iter->second->data = NULL;
        }
        
        if(iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
        
    }
}

bool Block::insertData(uint16_t idx, uint16_t offset, Data* data)
{
    if(data == NULL)
        return false;
    
    uint16_t dataSize = data->getDataSize();
  
    if(freeSpace < dataSize)
        return false;
    
    if(checkOffset(offset))  // offset 있으면 리턴
        return false;
    
    IndirectionData* iData = new IndirectionData(offset, data);
    
    int16_t lastIndirectionIdx = getLastIndirectionNumber();
    
    indirectionDataMap.insert(std::pair<uint16_t, IndirectionData*>(idx, iData));
    
    if(idx <= lastIndirectionIdx)
        calculateFreeSpace(dataSize, 0, true);
    else
        calculateFreeSpace(dataSize, 2, true);
    
    increaseIndirectionCnt();
    
    return true;
}

bool Block::deleteData(uint16_t idx)
{
    auto iter = indirectionDataMap.find(idx);
    
    if(iter == indirectionDataMap.end())
        return false; //찾는 키 없다.
    
    Data* data = iter->second->data;
    uint16_t dataSize = data->getDataSize();
    uint16_t freeSpaceSize = freeSpace + dataSize;
    
    if(BLOCK_FIRST_FREE_SPACE < freeSpaceSize)
        return false; // 데이터 삭제 하면 프리사이즈값이 오류남...
    
    
    if(iter->second->data != NULL)
    {
        delete iter->second->data;
        iter->second->data = NULL;
    }
    
    if(iter->second != NULL)
    {
        delete iter->second;
        iter->second = NULL;
    }
    
    
    indirectionDataMap.erase(iter);
    calculateFreeSpace(dataSize, 0, false);
    decreaseIndirctionCnt();
    
    return true;
}

bool Block::deleteData(std::string dataKey)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter )
        if(dataKey.compare(iter->second->data->getKey()) ==0)
            return deleteData(iter->first);
    
    return false;
}

Data* Block::getData(uint16_t idx)
{
    auto iter = indirectionDataMap.find(idx);
    
    if(iter == indirectionDataMap.end())
        return NULL; //찾는 데이터
    
    return iter->second->data;
}

Data* Block::getData(std::string dataKey)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter )
        if(dataKey.compare(iter->second->data->getKey()) ==0)
            return iter->second->data;
    
    return NULL;
}


uint64_t Block::getIndirectionBlockAdr(uint64_t blockAdr, uint16_t indirectionNumber)
{
    uint64_t firstIndBlockAdr = getFirstIndirectionBlockAdr(blockAdr);
    return firstIndBlockAdr + (indirectionNumber*2);
    
}


uint16_t Block::getNewOffset(uint16_t dataSize) // 컴팩션 했다고 가정하고 가져오는 새로운 offset이다.
{
    uint16_t minimumOffset = BLOCK_SIZE;
    
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter )
        if(iter->second->offset < minimumOffset)
            minimumOffset = iter->second->offset;
    
    uint16_t newOffset = minimumOffset - dataSize;
    
    
    return newOffset;
}


uint16_t Block::getLargestOffset(uint16_t limitValue)
{
    uint16_t maximum =0;
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter )
    {
        uint16_t curOffset = iter->second->offset;
        if((curOffset > maximum) && (curOffset <limitValue))
            maximum = curOffset;
    }
    
    return maximum;
}

uint16_t Block::getNewIndirectionNumber()
{
    int i =0;
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter )
    {
        if(i != iter->first)
            return i;
        ++i;
    }
    
    return i;
}

uint16_t Block::getIndNumByOffset(uint16_t offset)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
    {
        if(iter->second->offset == offset)
            return iter->first;
    }
    
    return 0;
}

uint16_t Block::getIndNumByKey(std::string componentKey)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
    {
        std::string key = iter->second->data->getKey();
        if(key.compare(componentKey) == 0)
            return iter->first;
    }
    
    return -1;
}

uint16_t Block::getOffsetByIndNum(uint16_t indNum)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
    {
        if(iter->first == indNum)
            return iter->second->offset;
    }
    
    return -1;
}

uint16_t Block::getOffsetByKey(std::string componentKey)
{
    for(auto iter = indirectionDataMap.begin(); iter != indirectionDataMap.end(); ++iter)
    {
        std::string key = iter->second->data->getKey();
        if(key.compare(componentKey) == 0)
            return iter->second->offset;
    }
    
    return 0;
}

uint16_t Block::getLargestDatasDistanceSize()
{
    std::vector<uint16_t> offsetList;  // 큰 오프셋 순으로 저장
    std::vector<uint16_t> indNumList;
 
    int limitOffset = BLOCK_SIZE;
    for(int i = 0; i < indirectionDataMap.size(); ++i)
    {
        uint16_t offset = getLargestOffset(limitOffset);
        offsetList.push_back(offset);
        limitOffset = offset;
    }
    
    for(uint16_t offset : offsetList)
    {
        uint16_t indirectionNum = getIndNumByOffset(offset);
        indNumList.push_back(indirectionNum);
    }
    
    std::vector<Data*> dataList;
    for(uint16_t indNum : indNumList)
    {
        Data* data = getData(indNum);
        dataList.push_back(data);
    }
    
    uint16_t largestDatasDis = 0;
    int necessaryIdx = 0;
    for(int i= 0 ; i < dataList.size(); ++i)
    {
        int nextIdx = i+1;
        if(nextIdx >= dataList.size())
            break;
        
       uint16_t datasDistance = offsetList[i] - (offsetList[nextIdx]+ dataList[nextIdx]->getDataSize());
        
        if(datasDistance <= largestDatasDis)
            continue;
            
        largestDatasDis = datasDistance;
        necessaryIdx = nextIdx;
        
    }
    
    return largestDatasDis;
    
}













