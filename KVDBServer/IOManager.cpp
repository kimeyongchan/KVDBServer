#include "IOManager.h"
#include "RequestInfo.h"
#include "Log.h"
#include "Block.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"
#include "KVDBServer.h"
#include <sstream>
#include "Defines.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}

// 기존 블럭 쓰거나, 체이닝할때 새블럭 할당
int8_t IOManager::processInsert(InsertRequestInfo* reqInfo)
{
    DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());
   
    /*
    insertNamedDataIndirectionAdrList.clear();
    insertBufferCacheDataMap.clear();
    NamedData* parentNamedData;
    uint64_t indirectionBlockAdr =0;
    
    // 먼저 있는지 확인한다.
    int namedCacheInsertComponentIdx =0; // 네임드캐시 넣을 네임드 데이터 컴퍼넌트 시작 인덱스
    std::vector<std::string> components = split(reqInfo->key, '/');
    NamedData* nd = namedCache->getRoodAddr();
    
    for (int i=0; i < components.size(); ++i)
    {
        std::string c = components[i];
        
        NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
        
        if(childNd == NULL)// 없을때
        {
            parentNamedData = nd;  // 네임드 캐시에 넣을때 쓰일 변수
            namedCacheInsertComponentIdx = i;  // 네임드 캐시에 넣을때 쓰일 변수
            int8_t result = checkBufferCacheAndDisk_1(nd->getIdtAddr(),i, components.size()-1);
            
            if(result <0)
                return result;
            else
                break;
        }
        
        if(i != components.size()-1)  // 있을때, 중간 노드 일때
        {
            nd = childNd;
            continue;
        }

        // 있을때 맨 마지막일 때
        int8_t result = checkBufferCacheAndDisk_1(childNd->getIdtAddr(),  i, components.size()-1);
        indirectionBlockAdr = childNd->getIndBlockAddress();
        
        if(result <0)
            return result;
        else
            break;
        
    }// for
    
    
    
    // freeSpace 공간 있으면 compaction 없으면 chaining new block
    uint64_t ba = ibaToBa(indirectionBlockAdr);
    uint16_t offsetIdx =ibaToOffsetIdx(indirectionBlockAdr, ba);
    
    Block* block = bufferCache->findBlock(ba);
    if(block == NULL)
    {
        block = new Block();
        if(KVDBServer::getInstance()->m_diskManager->readBlock(blockAdr, block)== false)
        {
            delete block;
            return -1; // 블럭이 없다.
        }
    }
    
    // 컴팩션 해야한다.
    compaction(block);
    
    // 프리스페이스와 실제 넣을값 비교해봐야한다.
    std::string key = components.back();
    uint16_t dataSize = sizeof(uint8_t) + key.size() + sizeof(uint32_t) + reqInfo->value.size();
    uint16_t newIndirectionNumber = block->getNewIndirectionNumber();
    uint16_t lastIndirectionNmber = block->getLastIndirectionNumber();
    std::map<uint64_t, Block*> chainingBlockMap;
    
    if(newIndirectionNumber > lastIndirectionNmber)
        dataSize += 2;
    
    std::vector<LogBufferInsertData> logBufferDataList;
    
    if(dataSize <= block->getFreeSpace()) // 블럭에 바로 넣을 수 있다.
    {
        KeyValueData* data = new KeyValueData();
        data->setFormatType(1); // 체이닝 없는 일반 데이터
        data->setKey(key);
        data->setValue(reqInfo->value);
        uint16_t newOffset = block->getNewOffset(data->getDataSize());
        
        block->insertData(newIndirectionNumber, newOffset, data);
        
        insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(ba, block));
        
        // 로그버퍼에 넣을 데이터
        uint64_t indBlockAdr = block->getIndirectionBlockAdr(ba, newIndirectionNumber);
        LogBufferInsertData logBufData(false, true, indBlockAdr, newOffset, data);
        logBufferDataList.push_back(logBufData);
        
        
    }else // chaining 필요하다.
    {
        uint16_t minimumBlockSize = sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t) + sizeof(uint64_t);
                                // formatType  +  key length     + key size    +  value length  +  chaining address
        
        if(minimumBlockSize > block->getFreeSpace())
            return -1; // 최소 체이닝 할수 있는 데이터 크기조차 넣을 수 없다.
        
        
        uint32_t valueSize = reqInfo->value.size();
        insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(ba, block));
        Block* preBlock = block;
        uint64_t logBufferIndBlockAdr = ba;
        while(valueSize > 0)
        {
            if(preBlock->getFreeSpace() >= (valueSize + sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t)))
            {
                KeyValueData* data = new KeyValueData();
                data->setFormatType(1); // 체이닝 없는 일반 데이터
                data->setKey(key);
                data->setValue(reqInfo->value);
                uint16_t newOffset = preBlock->getNewOffset(data->getDataSize());
                
                preBlock->insertData(preBlock->getNewIndirectionNumber(), newOffset, data);
                break;
            }
            
            uint64_t newChingingBlockAdr = cacheMgr->getNewBlockAdr();  // 캐시로 부터 새로운 주소를 받아온다.
            Block* newChainingBlock = new Block();
            
            KeyValueChainingData* data = new KeyValueChainingData();
            data->setFormatType(2); // 체이닝 데이터
            data->setKey(key);
            
            int realDataSize = preBlock->getFreeSpace() - data->getDataSize();
            valueSize -= realDataSize;
            std::string realValue =reqInfo->value.substr( reqInfo->value.size()-valueSize,realDataSize);
            data->setValue(realValue);
            
            uint64_t newBlockIndAdr = newChainingBlock->getFirstIndirectionBlockAdr(newChingingBlockAdr);
            data->setIndBlockAddress(newBlockIndAdr);  // 체이닝
            
            uint16_t newOffset = preBlock->getNewOffset(data->getDataSize());
            preBlock->insertData(preBlock->getNewIndirectionNumber(), newOffset, data); // 이때 freeSpace 줄어든다.
            
            if(valueSize >0)
                preBlock->setChainingAddress(newChingingBlockAdr);
            
            insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newChingingBlockAdr, newChainingBlock)); // 체이닝 블럭 넣기
            
            if(logBufferDataList.size() ==0)
            {
                // 로그버퍼에 넣을 데이터
                uint64_t indBlockAdr = block->getIndirectionBlockAdr(logBufferIndBlockAdr, preBlock->getNewIndirectionNumber());
                LogBufferInsertData logBufData(false, true, indBlockAdr, newOffset, data);
                logBufferDataList.push_back(logBufData);
                
            }else
            {
                // 로그버퍼에 넣을 데이터
                uint64_t indBlockAdr = block->getIndirectionBlockAdr(logBufferIndBlockAdr, preBlock->getNewIndirectionNumber());
                LogBufferInsertData logBufData(true, true, indBlockAdr, newOffset, data);
                logBufferDataList.push_back(logBufData);
            }
            
            preBlock = newChainingBlock;
            logBufferIndBlockAdr = newBlockIndAdr;
        }
        
    }
    
    // NamedCache , BufferCache에 넣기  더티 상태
    // NamedCache 넣기
    int k =0;
    for(int i= namedCacheInsertComponentIdx; i < components.size(); ++i )
    {
        NamedData* childNamedData = new NamedData(components[i], insertNamedDataIndirectionAdrList[k]);
        k++;
        
        namedCache->insert(parentNamedData, childNamedData);
        parentNamedData = childNamedData;
    }
    
    
    // BufferCache 넣기
    for (auto iter = insertBufferCacheDataMap.begin(); iter!= insertBufferCacheDataMap.end(); ++iter)
        bufferCache->insertBlock2Cache(iter->first, iter->second);
    
    
    // 로그버퍼에 넣는다
    for(LogBufferInsertData data: logBufferDataList)
        logBuffer->saveLog(data.isAllocateBlock, data.isInsert, data.indBlockAddress, data.offset, data.data); 
 

    // 로그파일에 넣는다.
    const char* log = logBuffer->readLogBuffer();
    logFile->writeLogFile(log);
    
    
    logBuffer->clear();
    
    
    // 캐시데이터 더티 정리 -> 하드에 실제 적기
    result = diskIo->insert(ba, block);
    if( result != SUCCESS)
    {
        // error return
    }
    
    bufferCache->delDirty(block);
    
    
    //    arrange dirty buffer cache
    
    
    logFile->clear();
    
    
    */
    
    return 0;
    
}


// 새블럭 생성
int8_t IOManager::processInsert(InsertDirectoryRequestInfo* reqInfo)
{
    
    DebugLog("INSERT_DIRECTORY - key : %s", reqInfo->key.c_str());
    
    return 0;
    
}


void IOManager::processFind(FindRequestInfo* reqInfo)
{
    
    DebugLog("FIND - key : %s", reqInfo->key.c_str());
    
    /*
     const NamedData* nd = namedCache->findND(component, NamedData); // for
     
     Block* block = bufferCache->findBlock(ba, component); //for
     
     if(block == NULL)
     {
     block = bufferCache->findBlockByList(key);// search all
     if(block == NULL)
     {
     block = diskIo->findBlock(ba); //for
     }
     }
     
     */
    
}


void IOManager::processDelete(DeleteRequestInfo* reqInfo)
{
    
    DebugLog("DELETE - key : %s", reqInfo->key.c_str());
    /*
     block ba key
     
     namedCache->deleteData(ND, component);
     bufferCache->DeleteData(block, component);
     
     compaction
     */
    
}


std::vector<std::string> IOManager::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    
    return elems;
}
/*
uint64_t IOManager::ibaToBa(uint64_t iba)
{
    uint64_t rootBlockadr= KVDBServer::getInstance()->cacheMgr->getSuperBlock()->getRootBlockAddress();
    
    uint64_t distance = (iba - rootBlockadr) % BLOCK_SIZE;
    
    return iba - distance;
}

uint16_t IOManager::ibaToOffsetIdx(uint64_t iba, uint64_t ba)
{
    uint8_t headerSize = Block::getBlockHeaderSize();
    uint16_t offsetIdx = (iba -(ba + headerSize) ) /2;
    return offsetIdx;
}


int8_t IOManager::checkBufferCacheAndDisk_1(uint64_t indirectionBa, int curIdx, int lastIdx)
{
    uint64_t ba = ibaToBa(indirectionBa);
    uint16_t offsetIdx =ibaToOffsetIdx(indirectionBa, ba);
    
    Block* block = bufferCache->findBlock(ba);
    Block*  diskReadBlock = NULL;
    if(block == NULL)
    {
        diskReadBlock = new diskReadBlock();
        if(KVDBServer::getInstance()->m_diskManager->readBlock(ba, diskReadBlock)== false)
        {
            delete diskReadBlock;
            return -1; // 블럭이 없다.
        }
            
        block = diskReadBlock;
    }
    
    insertNamedDataIndirectionAdrList.push_back(indirectionBa);   //// 네임드 캐시에 넣을때 인다이렉션 블럭어드레스 .... 러프하게 생각해서 다시한번 생각해 봐야한다.
    
    Data* data = block->getData(offsetIdx);
    if(curIdx != lastIdx)  // 중간 데이터 일때
    {
        if(diskReadBlock != NULL)
            insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(ba, diskReadBlock));
        
        if(data == NULL)
                return -2;
        else
        {
            DirectoryData* dirData = (DirectoryData*)data;
            return checkBufferCacheAndDisk(dirData->getIndBlockAddress(), curIdx +1, lastIdx);
        }
        
    }else // 마지막 데이터 일때
    {
        if(data == NULL)
            return 0; // insert 해도 된다.
        else
            return -3; // 값이 이미 있음 insert error
    }
    
}

*/

