#include "IOManager.h"
#include "RequestInfo.h"
#include "Log.h"
#include "Block.h"
#include "DirectoryData.h"
#include "KVDBServer.h"
#include <sstream>
#include "Defines.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}


int8_t IOManager::processInsert(InsertRequestInfo* reqInfo)
{
    
    DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());
   
    
    /*
    // 먼저 있는지 확인한다.
    //map<string, Block> buffCachInsertValMap;
    
    std::vector<std::string> components = split(reqInfo->key, '/');
    
    NamedData* nd = namedCache->getRootNamedData();
    
    for (int i=0; i < components.size(); ++i)
    {
        std::string c = components[i];
        
        NamedData* childNd = namedCache->findND(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
        
        
        if(childNd == NULL)// 없을때
        {
            int8_t result = checkBufferCacheAndDisk(nd->getIndBlockAddress(),i, components.size()-1);
            
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
        int8_t result =checkBufferCacheAndDisk(childNd->getIndBlockAddress(),  i, components.size()-1);
        
        if(result <0)
            return result;
        else
            break;
        
    }// for
    

    
    // 위에서 하드에 없는지 확인했으므로 실제로 넣는 작업한다.
    result = logBuffer->insert(reqInfo);
    if(result != SUCCESS)
        return -2;// error return
    
    
    
    
    for (auto& val : buffCachInsertValMap)
    {
        string component = val->key;
        Block block = val->get(component)
        bufferCache->insertData(component, block);  // (block, component, value); 였는데 바꿨음
    }
    
    
    bufferCache->setDirty(block);
    
    bufferCache->arangement();
    
    
    
    
    
    ba = DiskIo->RequestNewBlock();
    
    block = bufferCache->requestNewBlock(ba);
    
    namedCache->insert(component, IBA);
    
    namedCache->arangement();
    
    
    
    
    
    result = logFile->insert(reqInfo);
    if( result != SUCCESS)
    {
        // error return
    }
    
    
    logBuffer->clear();
    
    result = diskIo->insert(ba, block);
    if( result != SUCCESS)
    {
        // error return
    }
    
    bufferCache->delDirty(block);
    
    
    //    arrange dirty buffer cache
    
    
    logFile->clear();
    
    
    
    ////
    
    
    
    ////
    */
    // return send success
    
    return 0;
    
}


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


int8_t IOManager::checkBufferCacheAndDisk(uint64_t indirectionBa,  int curIdx, int lastIdx)
{
    uint64_t ba = ibaToBa(indirectionBa);
    uint16_t offsetIdx =ibaToOffsetIdx(indirectionBa, ba);
    
    
    Block* block = bufferCache->find(ba);
    Block readBlock;
    if(block == NULL)
    {
        if(KVDBServer::getInstance()->m_diskManager->readBlock(blockAdr, &readBlock)== false)
            return -1; // 블럭이 없다.
        
        block = &readBlock;
    }
    
    
    Data* data = block->getData(offsetIdx);
    if(curIdx != lastIdx)  // 중간 데이터 일때
    {
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

