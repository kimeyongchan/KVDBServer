
#include <sstream>
#include "IOManager.h"
#include "XmlData.h"
#include "RequestInfo.h"
#include "Log.h"
#include "LogBuffer.h"
#include "LogFile.h"
#include "Block.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"
#include "KVDBServer.h"
#include "Defines.h"
#include "DiskManager.h"
#include "SuperBlock.h"
#include "Network.h"
#include "NamedCache.h"
#include "bufferCache.h"
#include "radix_tree.h"
#include "LogBuffer.h"

#include "ServerMasterPacket.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}

void IOManager::connected(const ConnectInfo* connectInfo)
{
    DebugLog("connected");
    
    KVDBServer::getInstance()->network->finishProcessing(tfd, connectInfo);
}


void IOManager::disconnected(const ConnectInfo* connectInfo)
{
    DebugLog("disconnected");
    
    KVDBServer::getInstance()->network->finishProcessing(tfd, connectInfo);
}


void IOManager::receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{    
    if(connectInfo->serverModule == SERVER_MODULE_MASTER)
    {
        receiveMasterData(connectInfo, data, dataSize);
    }
    else if(connectInfo->serverModule == SERVER_MODULE_CLIENT)
    {
        receiveClientData(connectInfo, data, dataSize);
    }
    else
    {
        ErrorLog("invalid server module - %d", connectInfo->serverModule);
    }
}


void IOManager::receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    RequestInfo* requestInfo;
    
    if(parsingQuery(data, dataSize, &requestInfo) == false)
    {
        ErrorLog("parsing error");
        return ;
    }
    
    switch (requestInfo->type) {
        case INSERT_REQUEST:
        {
            InsertRequestInfo * iri = (InsertRequestInfo*)requestInfo;
            processInsert(iri);
            delete iri;
            break;
        }
        case INSERT_DIRECTORY_REQUEST:
        {
            InsertDirectoryRequestInfo * idri = (InsertDirectoryRequestInfo*)requestInfo;
            processInsert(idri);
            delete idri;
            break;
        }
        case FIND_REQUEST:
        {
            FindRequestInfo * fri = (FindRequestInfo*)requestInfo;
            processFind(fri);
            delete fri;
            break;
        }
        case DELETE_REQUEST:
        {
            DeleteRequestInfo * dri = (DeleteRequestInfo*)requestInfo;
            processDelete(dri);
            delete dri;
            break;
        }
        default:
        {
            ErrorLog("type - %d", requestInfo->type);
            break;
        }
    }
    
    KVDBServer::getInstance()->network->sendData(tfd, connectInfo, "fefe", 4);
}


void IOManager::receiveMasterData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    uint8_t commandType;
    memcpy(&commandType, data, sizeof(commandType));
    
    if(commandType == ServerMasterPacket::COMMAND_TYPE_OPEN_CLIENT_SOCK_REQ)
    {
        uint16_t port;
        memcpy(&port, data + commandType, sizeof(port));
        NetworkInfo networkInfo;
        networkInfo.type = SERVER_TYPE_SERVER;
        networkInfo.module = SERVER_MODULE_CLIENT;
        memset(networkInfo.ip, 0, MAX_IP_LEN + 1);
        memcpy(networkInfo.ip, "127.0.0.1", 9);
        networkInfo.port = port;
        
        if(KVDBServer::getInstance()->network->AddNetworkInfo(&networkInfo) == false)
        {
            ErrorLog("network add false");
            return ;
        }
        
        ServerMasterPacket::OpenClientSockRes ocsr;
        memset(ocsr.ip, 0, sizeof(MAX_IP_LEN + 1));
        memcpy(ocsr.ip, "127.0.0.1", 9);
        ocsr.port = port;
        
        char sendArray[sizeof(ocsr)];
        memcpy(sendArray, &ocsr, sizeof(ocsr));
        KVDBServer::getInstance()->network->sendData(tfd, connectInfo, sendArray, sizeof(ocsr));
        DebugLog("Send ");
    }
    else
    {
        ErrorLog("invalid commandType - %d", commandType);
    }
}

bool IOManager::parsingQuery(const char* query, int queryLen, RequestInfo** pri) // pasing query to requestInfo
{
    std::string queryString(query, queryLen);
    
    std::size_t findTypeLength = queryString.find_first_of('(');
    
    if(findTypeLength == std::string::npos)
    {
        return false;
    }
    
    std::size_t findDataLength = queryString.find_first_of(')');
    
    
    if(queryString.substr(findDataLength+ 1).compare(";") != 0)
    {
        return false;
    }
    
    if(findDataLength == std::string::npos)
    {
        return false;
    }
    
    if(findTypeLength + 2 >= findDataLength)
    {
        return false;
    }
    std::string ee = queryString.substr(findTypeLength + 1, 1);
    if(queryString.substr(findTypeLength + 1, 1).compare("\"") != 0
       || queryString.substr(findDataLength + -1, 1).compare("\"") != 0)
    {
        ErrorLog("\" error ");
        return false;
    }
    
    std::string typeString = queryString.substr(0, findTypeLength);
    std::string dataString = queryString.substr(findTypeLength + 1, findDataLength - findTypeLength - 1);
    
    if(typeString.compare("insert") == 0) // insert
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.substr(findDQM + 1, 2).compare(",\"") == 0) // file insert
        {
            std::size_t findDQM2 = dataString.find_first_of('"', findDQM + 3);
            
            if(findDQM2 == std::string::npos)
            {
                return false;
            }
            
            std::string secondString = dataString.substr(findDQM + 3, findDQM2 - findDQM - 3);
            
            if(secondString.empty())
            {
                return false;
            }
            
            if (dataString.length() != findDQM2 + 1)
            {
                return false;
            }
            
            InsertRequestInfo* iri = new InsertRequestInfo();
            iri->key = firstString.substr();
            iri->value = secondString.substr();
            
            *pri = iri;
            
            return true;
            
        }
        else if(dataString.length() == findDQM + 1) //directory insert
        {
            InsertDirectoryRequestInfo* idri = new InsertDirectoryRequestInfo();
            idri->key = firstString.substr();
            
            *pri = idri;
            
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(typeString.compare("find") == 0) // find
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.length() != findDQM + 1)
        {
            return false;
        }
        
        FindRequestInfo* fri = new FindRequestInfo();
        fri->key = firstString.substr();
        
        *pri = fri;
        
        return true;
    }
    else if(typeString.compare("delete") == 0) // delete
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.length() != findDQM + 1)
        {
            return false;
        }
        
        DeleteRequestInfo* dri = new DeleteRequestInfo();
        dri->key = firstString.substr();
        
        *pri = dri;
        
        return true;
    }
    else
    {
        return false;
    }
    
    return true;
}



int8_t IOManager::processInsert(InsertRequestInfo* reqInfo)
{
    NamedCache*  namedCache  = KVDBServer::getInstance()->namedCache;
    BufferCache* bufferCache = KVDBServer::getInstance()->bufferCache;
    LogBuffer*   logBuffer   = KVDBServer::getInstance()->logBuffer;
    LogFile*     logFile     = KVDBServer::getInstance()->logFile;
 
    lastBlockChainingAdrList.clear();
    componentList.clear();
    namedCacheDataList.clear();                         // 네임드캐시
    insertBufferCacheDataMap.clear();                   // 버퍼캐시
    std::vector<DirtyBlockInfo> dirtyBlockInfoList;     // 로깅, 디스크
  
    NamedData*  parentNamedData = NULL;
    uint64_t    indirectionBlockAdr =0;
    componentList = split(reqInfo->key, '/');
   
    IoMgrReturnValue returnVal(NULL, 0, 0);  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
    NamedData* nd = namedCache->getRootAddr();
    
    for (int i = 0; i < componentList.size(); ++i)
    {
        std::string c = componentList[i];
        NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
        
        if(childNd == NULL)// 없을때
        {
            returnVal = checkBufferCacheAndDisk(nd->getBlockAddress(), i-1, componentList.size()-1);
            parentNamedData = nd;
            
            if(returnVal.returnCode <0)
            {
                caching(parentNamedData);
                return returnVal.returnCode;
            }
            else
            {
                if(namedCacheDataList.size() > 0)
                    indirectionBlockAdr = namedCacheDataList.back().indirectionBlockAdr;
                break;
            }
        }
        
        if(i < componentList.size()-1)  // 있을때, 중간 노드 일때
        {
            nd = childNd;
            continue;
        }
        
        // 있을때 맨 마지막일 때
        returnVal = checkBufferCacheAndDisk(childNd->getBlockAddress(), i, componentList.size()-1);
        parentNamedData = nd;  // 네임드 캐시에 넣을때 쓰일 변수
 
        if(returnVal.returnCode <0)
        {
            caching(parentNamedData);
            return returnVal.returnCode;
        }
        else
        {
            indirectionBlockAdr = childNd->getBlockAddress();
            
            break;
        }
 
        
    }// for end
    

    Block*      block    = returnVal.block;
    uint64_t    blockAdr = ibaToBa(indirectionBlockAdr);
    std::string key      = componentList[returnVal.componentIdx];
    
    // 블럭의 프리스페이스와 실제 넣을 데이터 사이즈를 비교해봐야한다.
    // 실제 넣을 데이터 사이즈를 구한다.
    
    uint16_t addingOffsetByte = 0;
    if(block->getNewIndirectionNumber() > block->getLastIndirectionNumber()) // 인다이렉션주소를 새로 추가하면 사이즈 2byte 추가 해야 한다.
        addingOffsetByte += 2;
    
    uint16_t dataSize = sizeof(uint8_t) + key.size() + sizeof(uint32_t) + reqInfo->value.size() + addingOffsetByte;
    
    
    Block* lastBlock = NULL;
    uint64_t lastBlockAdr =0;
    for(uint64_t ba: lastBlockChainingAdrList)  // 체이닝된 블럭들 보면서 데이터 넣을 수 있는 블럭 찾는다.
    {
        lastBlock = bufferCache->findBlock(ba);
        if(lastBlock == NULL)
        {
            auto iter =insertBufferCacheDataMap.find(ba);
            lastBlock = iter->second;
        }
        
        if(lastBlock == NULL)
            continue;
        
        if( dataSize <= lastBlock->getFreeSpace())
        {
            lastBlockAdr = ba;
            break;
            
        }else
            lastBlock = NULL;
    }
    
    
    if(lastBlock != NULL) // 블럭에 바로 넣을 수 있다.
    {
        block = lastBlock;
        blockAdr = lastBlockAdr;
        
        KeyValueData* data = new KeyValueData();
        data->setFormatType(FLAG_KEY_VALUE_DATA);
        data->setKey(key);
        data->setValue(reqInfo->value);
        
        if( data->getDataSize() > block->getLargestDatasDistanceSize()) // 컴팩션 여부 확인
            compaction(block);
        
        uint16_t offset = block->getNewOffset(data->getDataSize());
        uint16_t indNum = block->getNewIndirectionNumber();
        
        // 블럭에 데이터 넣기
        block->insertData(indNum, offset, data);
        
        // 로그버퍼에 넣을 데이터
        dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, false, true, blockAdr, indNum));
    
    }else // chaining 필요하다.
    {
        
        Block*   curBlock       = block;
        uint64_t curIndBlockAdr = indirectionBlockAdr;
        uint64_t curBlockAdr    = blockAdr;
        uint64_t prevBlockAdr   = 0;
        bool     isAllockBlock  = false;
        
        uint16_t minimumBlockSize = sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t) + sizeof(uint64_t) + addingOffsetByte;
                                    // formatType  +  key length     + key size    +  value length  +  chaining address
        
        if(minimumBlockSize > block->getFreeSpace())
        {
            uint64_t newFirstBlockAdr   = bufferCache->newBlock();
            Block*   newFirstBlock      = new Block();
            uint16_t firstIndBlockAdr   = newFirstBlock->getFirstIndirectionBlockAdr(newFirstBlockAdr);
            
            block->setChainingAddress(newFirstBlockAdr);
            dirtyBlockInfoList.push_back(DirtyBlockInfo(curBlock, isAllockBlock, false, true, curBlockAdr, curIndBlockAdr, 0, newFirstBlockAdr));
            
            curBlock        = newFirstBlock;
            curIndBlockAdr  = firstIndBlockAdr;
            curBlockAdr     = newFirstBlockAdr;
            prevBlockAdr    = blockAdr;
            isAllockBlock   = true;
            
            // 버퍼캐시에 넣을 데이터
            insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newFirstBlockAdr, newFirstBlock)); // 체이닝 블럭 넣기
        }
        
        uint32_t remainValueSize = (uint32_t)reqInfo->value.size();
         while(remainValueSize > 0)
         {
             if(curBlock->getFreeSpace() >=
                (remainValueSize + sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t) + addingOffsetByte))
             {
                 KeyValueData* data = new KeyValueData();
                 data->setFormatType(FLAG_KEY_VALUE_DATA); // 체이닝 없는 일반 데이터
                 data->setKey(key);
                 
                 std::string curBlockValue =reqInfo->value.substr( reqInfo->value.size()- remainValueSize, remainValueSize);
                 data->setValue(curBlockValue);
                 remainValueSize = 0;
                 
                 uint16_t newOffset = curBlock->getNewOffset(data->getDataSize());
                 curBlock->insertData(curBlock->getNewIndirectionNumber(), newOffset, data);
                 
                 uint16_t indNum = ibaToOffsetIdx(curIndBlockAdr, curBlockAdr);
                 dirtyBlockInfoList.push_back(DirtyBlockInfo(curBlock, isAllockBlock, false, true, curBlockAdr, indNum, prevBlockAdr));
                 
                 break;
             }
             
             
             // 블럭에 넣을 데이터 생성
             KeyValueChainingData* data = new KeyValueChainingData();
             data->setFormatType(FLAG_KEY_VALUE_CHAINING_DATA); // 체이닝 데이터
             data->setKey(key);
      
             int curBlockValueSize = curBlock->getFreeSpace() - (data->getDataSize()+ addingOffsetByte);
             std::string curBlockValue =reqInfo->value.substr( reqInfo->value.size()-remainValueSize,curBlockValueSize);
             data->setValue(curBlockValue);
             remainValueSize -= curBlockValueSize;
             
             
             uint64_t newChingingBlockAdr = bufferCache->newBlock();  // 캐시로 부터 새로운 주소를 받아온다.
             Block* newChainingBlock = new Block();
             uint64_t newBlockIndAdr = newChainingBlock->getFirstIndirectionBlockAdr(newChingingBlockAdr);
             data->setIndBlockAddress(newBlockIndAdr);
             
             
             // 블럭에 데이터 insert
             uint16_t curBlockNewOffset = curBlock->getNewOffset(data->getDataSize());
             curBlock->insertData(curBlock->getNewIndirectionNumber(), curBlockNewOffset, data); // 이때 freeSpace 줄어든다.
             
             curBlock->setChainingAddress(newChingingBlockAdr);  // 블럭에 체이닝 블럭 주소 세팅 해준다.
             
             // 버퍼캐시에 넣을 데이터
             insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newChingingBlockAdr, newChainingBlock)); // 체이닝 블럭 넣기
             
             uint16_t indNum_ = ibaToOffsetIdx(curIndBlockAdr, curBlockAdr);
             dirtyBlockInfoList.push_back(DirtyBlockInfo(curBlock, isAllockBlock, false, true, curBlockAdr, indNum_,
                                                         prevBlockAdr, newChingingBlockAdr));
             
             curBlock               = newChainingBlock;
             curIndBlockAdr         = newBlockIndAdr;
             curBlockAdr            = newChingingBlockAdr;
             isAllockBlock          = true;
             prevBlockAdr           = curBlockAdr;
             addingOffsetByte       = 2;
             
         }// while end
     }// else
     
    // 캐싱한다
    caching(parentNamedData);

    // 로그버퍼에 넣는다
    for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
    {
        if(blockInfo.isLoging == false)
            continue;
        
        uint64_t   blockAdr  = blockInfo.blockAddress;
        uint16_t   freeSpace = blockInfo.block->getFreeSpace();
        uint16_t   indNum    = blockInfo.indirectionNum;
        uint16_t   offset    = blockInfo.block->getOffsetByIndNum(indNum);
        Data*      data      = blockInfo.block->getData(indNum);
        
        if(data != NULL)
            KVDBServer::getInstance()->logBuffer->saveLog(blockInfo.isAllocateBlock,blockInfo.isFreeBlock, blockInfo.isInsert,
                                                      blockAdr, freeSpace, indNum ,offset, data,
                                                      blockInfo.prevBlockAddress, blockInfo.nextBlockAddress);
    }
    
    // 로그파일에 쓴다
    char* log;
    logBuffer->commitLogBuffer(&log);
    logFile->writeLogFile((int)strlen(log), log);
    
    
    //디스크에 쓴다.
    for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
    {
        Block* block = blockInfo.block;
        uint64_t blockAdr = blockInfo.blockAddress;
        
        if(blockInfo.isAllocateBlock == true)
        {
            char* bitArray = KVDBServer::getInstance()->superBlock->getUsingBlockBitArray();
            KVDBServer::getInstance()->diskManager->writeBitArray(bitArray);
            
            if(block->getIndirectionDataMapSize()==0)
                continue;
        }
        
        KVDBServer::getInstance()->diskManager->writeBlock(blockAdr,block);
        blockInfo.block->setDirty(false);
    }
    
    // 로그파일 클리어
    logFile->clear();
    
     return 0;
 }
 
 
 // 새블럭 생성
 int8_t IOManager::processInsert(InsertDirectoryRequestInfo* reqInfo)
 {
     
     NamedCache*  namedCache  = KVDBServer::getInstance()->namedCache;
     BufferCache* bufferCache = KVDBServer::getInstance()->bufferCache;
     LogBuffer*   logBuffer   = KVDBServer::getInstance()->logBuffer;
     LogFile*     logFile     = KVDBServer::getInstance()->logFile;
     
     lastBlockChainingAdrList.clear();
     componentList.clear();
     insertBufferCacheDataMap.clear();
     namedCacheDataList.clear();
     std::vector<DirtyBlockInfo> dirtyBlockInfoList;
     
     NamedData* parentNamedData = NULL;
     uint64_t   indirectionBlockAdr =0;
     IoMgrReturnValue returnVal(NULL, 0, 0);
     
     componentList= split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRootAddr();
     

     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             returnVal = checkBufferCacheAndDisk(nd->getBlockAddress(), i-1, componentList.size()-1);
             parentNamedData = nd;
             
             if(returnVal.returnCode < 0)
             {
                 caching(parentNamedData);
                 return returnVal.returnCode;
             }
             else
             {
                 if(namedCacheDataList.size() > 0)
                     indirectionBlockAdr = namedCacheDataList.back().indirectionBlockAdr;
                 break;
             }
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = checkBufferCacheAndDisk(childNd->getBlockAddress(), i, componentList.size()-1);
         parentNamedData = nd;
 
         if(returnVal.returnCode <0)
         {
             caching(parentNamedData);
             return returnVal.returnCode;
         }
         else
         {
             indirectionBlockAdr = childNd->getBlockAddress();
             break;
         }
         
     }// for
    
 
     // 새블럭 만들어서 찾은 마지막 블럭 데이터에 새블럭 첫번째 인다이렉션블럭주소를 저장시켜줘야 한다.
     //Block*   block    = returnVal.block;
     uint64_t blockAdr = ibaToBa(indirectionBlockAdr);
     
     // 1. 새블럭을 만들어준다.
     uint64_t newBlockAdr = bufferCache->newBlock();
     Block* newBlock = new Block();
     uint64_t newBlockFirstIndBa = newBlock->getFirstIndirectionBlockAdr(newBlockAdr);
     
     // 2. 새블럭의 첫번째 인다이렉션 주소를 갖는 디렉토리형 데이터를 생성한다.
     DirectoryData* data = new DirectoryData();
     data->setKey(componentList.back());
     data->setIndBlockAddress(newBlockFirstIndBa);
     
     
     // 3. 마지막 블럭(체이닝 블럭포함) FreeSize 넣을 수 있는 곳 찾는다.
     Block* lastBlock = NULL;
     uint64_t lastBlockAdr =0;
     for(uint64_t ba: lastBlockChainingAdrList)
     {
         lastBlock = bufferCache->findBlock(ba);
         if(lastBlock == NULL)
         {
             auto iter =insertBufferCacheDataMap.find(ba);
             lastBlock = iter->second;
         }
         
         if(lastBlock == NULL)
             continue;
         
         if(data->getDataSize() < lastBlock->getFreeSpace())
         {
             lastBlockAdr = ba;
             break;
         }else
             lastBlock = NULL;
     }
     
     
     
     if(lastBlock == NULL) // 데이터 넣을 블럭 없으면 새로운 블럭 생성
     {
         uint64_t newLastBlockAdr = bufferCache->newBlock();
         Block*   newLastBlock = new Block();
         uint16_t offSet = newLastBlock->getNewOffset(data->getDataSize());
         uint16_t offsetIdx = newLastBlock->getNewIndirectionNumber();
         newLastBlock->insertData(offsetIdx, offSet, data);
         
         returnVal.block->setChainingAddress(newLastBlockAdr);  // 마지막 블럭에 새로 생성한 마지막 블럭의 주소를 체이닝 해준다.
         
         // 캐시버퍼용 데이터
         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newLastBlockAdr, newLastBlock));
         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newBlockAdr, newBlock));
    
         // 로깅, 디스크 용
         dirtyBlockInfoList.push_back(DirtyBlockInfo(returnVal.block, false, false, true, blockAdr, 0, 0, newLastBlockAdr));
         dirtyBlockInfoList.push_back(DirtyBlockInfo(newLastBlock, true, false, true, newLastBlockAdr, offsetIdx, blockAdr, 0));
         dirtyBlockInfoList.push_back(DirtyBlockInfo(newBlock, true, false, true, newBlockAdr, 0, 0, 0, false)); // 로깅은 안한다
         
         
     }else // 기존 블럭에 넣을 수 있으면 컴팩션 여부 확인한다.
     {
         if(data->getDataSize() > lastBlock->getLargestDatasDistanceSize())
             compaction(lastBlock);
         
         uint16_t offset      = lastBlock->getNewOffset(data->getDataSize());
         uint16_t offsetIdx   = lastBlock->getNewIndirectionNumber();
         lastBlock->insertData(offsetIdx, offset, data);
         
         // 캐시버퍼용
         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newBlockAdr, newBlock));
         
         // 로깅, 디스크 용
         dirtyBlockInfoList.push_back(DirtyBlockInfo(lastBlock, false, false, true, lastBlockAdr, offsetIdx));
         dirtyBlockInfoList.push_back(DirtyBlockInfo(newBlock, true, false, true, newBlockAdr, 0, 0, 0, false)); // 로깅 안한다.
     }
     
     // 캐싱한다
     caching(parentNamedData);
     
     
     // 로그버퍼에 넣는다
     for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
     {
         if(blockInfo.isLoging == false)
             continue;
         
         uint64_t   blockAdr  = blockInfo.blockAddress;
         uint16_t   freeSpace = blockInfo.block->getFreeSpace();
         uint16_t   indNum    = blockInfo.indirectionNum;
         uint16_t   offset    = blockInfo.block->getOffsetByIndNum(indNum);
         Data*      data      = blockInfo.block->getData(indNum);
         
         KVDBServer::getInstance()->logBuffer->saveLog(blockInfo.isAllocateBlock,blockInfo.isFreeBlock, blockInfo.isInsert,
                                                       blockAdr, freeSpace, indNum ,offset, data,
                                                       blockInfo.prevBlockAddress, blockInfo.nextBlockAddress);
     }
     
     // 로그파일에 쓴다
     char* log;
     logBuffer->commitLogBuffer(&log);
     logFile->writeLogFile((int)strlen(log), log);
     
     
     //디스크에 쓴다.
     for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
     {
         Block* block = blockInfo.block;
         uint64_t blockAdr = blockInfo.blockAddress;
         
         if(blockInfo.isAllocateBlock)
         {
             char* bitArray = KVDBServer::getInstance()->superBlock->getUsingBlockBitArray();
             KVDBServer::getInstance()->diskManager->writeBitArray(bitArray);
             
             if(block->getIndirectionDataMapSize()==0)
                 continue;
         }
         
         KVDBServer::getInstance()->diskManager->writeBlock(blockAdr,block);
         blockInfo.block->setDirty(false);
     }
     
     Block* testBlock = new Block();
     KVDBServer::getInstance()->diskManager->readBlock(lastBlockAdr, testBlock);
     delete testBlock;
     
     // 로그파일 클리어
     logFile->clear();
      return 0;
 }
 
 
 void IOManager::processFind(FindRequestInfo* reqInfo)
 {
     
     BufferCache* bufferCache = KVDBServer::getInstance()->bufferCache;
     NamedCache*  namedCache  = KVDBServer::getInstance()->namedCache;
 
     componentList.clear();
     insertBufferCacheDataMap.clear();
     namedCacheDataList.clear();

     NamedData* parentNamedData;
     IoMgrReturnValue returnVal(NULL, 0, 0);  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
     
     // 먼저 있는지 확인한다.
     componentList = split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRootAddr();
     
     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             parentNamedData = nd;
             returnVal = findBufferCacheAndDisk(nd->getBlockAddress(),i-1, componentList.size()-1);
             
             if(returnVal.block == NULL) // 못찾았을 경우
             {
                 DebugLog("can not find %s", componentList[returnVal.componentIdx].c_str());
                 
             }else  // 찾았을 경우
             {
                 Block* findBlock = returnVal.block;
                 Data*  data      = findBlock->getData(componentList[returnVal.componentIdx]);
                 int8_t dataType  = data->getFormatType();
                 
                 if(dataType == FLAG_DIRECTORY_DATA)
                 {
                     DebugLog("find directory data key: %s", data->getKey().c_str());
                     
                 }else if(dataType == FLAG_KEY_VALUE_DATA)
                 {
                     KeyValueData* keyValData = (KeyValueData*)data;
                     DebugLog("find key vlue data key: %s, value: %s", keyValData->getKey().c_str(), keyValData->getValue().c_str() );
                     
                 }else if(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
                 {
                     
                     std::string value;
                     std::string key  = data->getKey();
                     KeyValueChainingData* chainingData        =(KeyValueChainingData*)data;
                     uint64_t              chainingIndBlockAdr = chainingData->getIndBlockAddress();
                     uint64_t              chainingBlockAdr    = ibaToBa(chainingIndBlockAdr);
                     
                     
                     while(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
                     {
                         chainingData        =(KeyValueChainingData*)data;
                         chainingIndBlockAdr = chainingData->getIndBlockAddress();
                         chainingBlockAdr    = ibaToBa(chainingIndBlockAdr);
                         
                         value += chainingData->getValue();
                         data = NULL;
                         
                         Block* chainingBlock = bufferCache->findBlock(chainingBlockAdr);
                         if(chainingBlock == NULL)
                         {
                             auto iter =insertBufferCacheDataMap.find(chainingBlockAdr);
                             if(iter != insertBufferCacheDataMap.end())
                                 chainingBlock =iter->second;
                             else
                             {
                                 chainingBlock = new Block();
                                 if(false == KVDBServer::getInstance()->diskManager->readBlock(chainingBlockAdr, chainingBlock))
                                 {
                                     delete chainingBlock;
                                     chainingBlock = NULL;
                                 }
                                 
                                 if(chainingBlock != NULL)
                                     insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(chainingBlockAdr, chainingBlock));
                             }
                         }
                         
                         if(chainingBlock != NULL)
                         {
                             uint16_t indNum = ibaToOffsetIdx(chainingIndBlockAdr, chainingBlockAdr);
                             data = chainingBlock->getData(indNum);
                             
                             if(data == NULL)
                                 break;
                         }
                         
                     }
                     
                     
                     if(data != NULL)
                     {
                         KeyValueData* kvData =(KeyValueData*)data;
                         value += kvData->getValue();
                     }
                     
                     
                     DebugLog("find key vlue chaining data key: %s, value: %s", key.c_str(), value.c_str());
                 }
             }
             
             caching(parentNamedData);
             break;
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = findBufferCacheAndDisk(childNd->getBlockAddress(),  i, componentList.size()-1);
         
         if(returnVal.block == NULL) // 못찾았을 경우
         {
             DebugLog("can not find %s", componentList[returnVal.componentIdx].c_str());
             caching(parentNamedData);
             break;
             
         }else  // 찾았을 경우
         {
             Block* findBlock = returnVal.block;
             Data* data =findBlock->getData(componentList[returnVal.componentIdx]);
             int8_t dataType =data->getFormatType();
             
             if(dataType == FLAG_DIRECTORY_DATA)
             {
                 DebugLog("find directory data key: %s", data->getKey().c_str());
                 
             }else if(dataType == FLAG_KEY_VALUE_DATA)
             {
                 KeyValueData* keyValData = (KeyValueData*)data;
                 DebugLog("find key vlue data key: %s, value: %s", keyValData->getKey().c_str(), keyValData->getValue().c_str() );
                 
             }else if(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
             {
                
                 std::string value;
                 std::string key  = data->getKey();
                 KeyValueChainingData* chainingData        =(KeyValueChainingData*)data;
                 uint64_t              chainingIndBlockAdr = chainingData->getIndBlockAddress();
                 uint64_t              chainingBlockAdr    = ibaToBa(chainingIndBlockAdr);
                 
                 
                 while(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
                 {
                     chainingData        =(KeyValueChainingData*)data;
                     chainingIndBlockAdr = chainingData->getIndBlockAddress();
                     chainingBlockAdr    = ibaToBa(chainingIndBlockAdr);
                     
                     value += chainingData->getValue();
                     data = NULL;
                     
                     Block* chainingBlock = bufferCache->findBlock(chainingBlockAdr);
                     if(chainingBlock == NULL)
                     {
                         auto iter =insertBufferCacheDataMap.find(chainingBlockAdr);
                         if(iter != insertBufferCacheDataMap.end())
                             chainingBlock =iter->second;
                         else
                         {
                             chainingBlock = new Block();
                             if(false == KVDBServer::getInstance()->diskManager->readBlock(chainingBlockAdr, chainingBlock))
                             {
                                 delete chainingBlock;
                                 chainingBlock = NULL;
                             }
                             
                             if(chainingBlock != NULL)
                                 insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(chainingBlockAdr, chainingBlock));
                         }
                     }
                     
                     if(chainingBlock != NULL)
                     {
                         uint16_t indNum = ibaToOffsetIdx(chainingIndBlockAdr, chainingBlockAdr);
                         data = chainingBlock->getData(indNum);
                         
                         if(data == NULL)
                             break;
                     }
        
                 }
                 
                 
                 if(data != NULL)
                 {
                     KeyValueData* kvData =(KeyValueData*)data;
                     value += kvData->getValue();
                 }
                 
                 
                 DebugLog("find key vlue chaining data key: %s, value: %s", key.c_str(), value.c_str());
             }
             
             caching(parentNamedData);
             break;
         }
     }// for

 }


 void IOManager::processDelete(DeleteRequestInfo* reqInfo)
 {

     NamedCache*  namedCache  = KVDBServer::getInstance()->namedCache;
     BufferCache* bufferCache = KVDBServer::getInstance()->bufferCache;
     LogBuffer*   logBuffer   = KVDBServer::getInstance()->logBuffer;
     LogFile*     logFile     = KVDBServer::getInstance()->logFile;
     
     componentList.clear();
     namedCacheDataList.clear();                         // 네임드캐시
     insertBufferCacheDataMap.clear();                   // 버퍼캐시
     std::vector<DirtyBlockInfo> dirtyBlockInfoList;
     
     NamedData* parentNamedData = NULL;
     NamedData* deleteNamedData = NULL;
     uint64_t indirectionBlockAdr =0;
     IoMgrReturnValue returnVal(NULL, 0, 0);  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
     
     // 먼저 있는지 확인한다.
     componentList = split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRootAddr();
     
     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             returnVal = findBufferCacheAndDisk(nd->getBlockAddress(), i-1, componentList.size()-1);
             parentNamedData = nd;
             
             if(returnVal.returnCode <0)
             {
                 caching(parentNamedData);
                 return;
             }
             else
             {
                 if(namedCacheDataList.size() > 0)
                 {
                     indirectionBlockAdr = namedCacheDataList.back().indirectionBlockAdr;
                     namedCacheDataList.pop_back();
                 }
                 break;
             }
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = findBufferCacheAndDisk(childNd->getBlockAddress(), i, componentList.size()-1);
         parentNamedData = nd;
         
         if(returnVal.returnCode <0)
         {
             caching(parentNamedData);
             return;
         }
         else
         {
             indirectionBlockAdr = childNd->getBlockAddress();
             deleteNamedData = nd;
             break;
         }
         
     }// for
     

     // 블럭에서 지울 데이터 저장해놓기
     std::string delComponent = componentList[returnVal.componentIdx];
     
     Block*     block     = returnVal.block;
     uint64_t   blockAdr  = ibaToBa(indirectionBlockAdr);
     uint16_t   indNum    = block->getIndNumByKey(delComponent);
     uint16_t   offset    = block->getOffsetByIndNum(indNum);
     Data*      data      = block->getData(indNum);
     uint8_t    type      = data->getFormatType();
     
     
     // 삭제할 수 있는 데이터인지 확인한다.
     if(type == FLAG_DIRECTORY_DATA)
     {
         DirectoryData* dirData   =(DirectoryData*) data;
         uint64_t indBlockAddress = dirData->getIndBlockAddress();
         uint64_t blockAddress    = ibaToBa(indBlockAddress);
         uint16_t offsetIdx       = ibaToOffsetIdx(indirectionBlockAdr, blockAddress);
         
         Block* checkBlock = bufferCache->findBlock(blockAddress);
         if(checkBlock == NULL)
         {
             checkBlock = new Block();
             if(false == KVDBServer::getInstance()->diskManager->readBlock(blockAddress, checkBlock))
                 delete checkBlock;
             else
                 if(checkBlock->getData(offsetIdx) != NULL)
                     return; // 데이터를 삭제할 수 없다.
         }
     }
     
     
   
     Block* prevBlock = NULL;
     uint64_t prevBlockAdr =0;
     for(uint64_t ba: lastBlockChainingAdrList)
     {
         if(ba == blockAdr)
             break;
         
         prevBlock = bufferCache->findBlock(ba);
         if(prevBlock == NULL)
         {
             auto iter =insertBufferCacheDataMap.find(ba);
             prevBlock = iter->second;
         }
         
         if(prevBlock == NULL)
             continue;
         
         prevBlockAdr = ba;
     }
     
     
     if(type == FLAG_KEY_VALUE_DATA)
     {
         if(block->getIndirectionDataMapSize() <= 1)  // 블럭을 삭제 해야 하는 경우
         {
             Block* nextBlock = NULL;
             uint64_t nextChainBlockAdr = block->getChaingAddress();
             if(nextChainBlockAdr > 0)
             {
                 nextBlock = bufferCache->findBlock(nextChainBlockAdr);
                 if(nextBlock == NULL)
                 {
                     nextBlock = new Block();
                     if(false == KVDBServer::getInstance()->diskManager->readBlock(nextChainBlockAdr, nextBlock))
                     {
                         delete block;
                         nextBlock = NULL;

                     }else
                         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(nextChainBlockAdr, nextBlock));
                 }
             }
             
             
             if(prevBlock != NULL)
             {
                 prevBlock->setChainingAddress(nextChainBlockAdr);
                 dirtyBlockInfoList.push_back(DirtyBlockInfo(prevBlock, false, false, false, prevBlockAdr, 0,0,0,false));// 로깅안함
             }
             
             dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, true, false, blockAdr, indNum,prevBlockAdr,nextChainBlockAdr));
             

             
         }else // 블럭의 데이터만 삭제하는 경우
             dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, false, false, blockAdr, indNum));
         

     }else
     {
         while(type == FLAG_KEY_VALUE_CHAINING_DATA)
         {
             
             KeyValueChainingData* chainingData        =(KeyValueChainingData*)data;
             uint64_t              chainingIndBlockAdr = chainingData->getIndBlockAddress();
             uint64_t              chainingBlockAdr    = ibaToBa(chainingIndBlockAdr);
             
             if(block->getIndirectionDataMapSize() <= 1)  // 블럭을 삭제 해야 하는 경우
             {
                 if(prevBlock != NULL)
                 {
                     prevBlock->setChainingAddress(chainingBlockAdr);
                     
                     bool hasPrevBlock = false;
                     for(DirtyBlockInfo dirtyBlock : dirtyBlockInfoList)
                     {
                         if( dirtyBlock.block == prevBlock)
                             hasPrevBlock = true;
                     }
                     
                     if(hasPrevBlock == false)
                         dirtyBlockInfoList.push_back(DirtyBlockInfo(prevBlock, false, false, false, prevBlockAdr, 0,0,0,false));
                 }
                 
                 dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, true, false, blockAdr, indNum,prevBlockAdr,chainingBlockAdr));
                 
             }else
             {
                 dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, false, false, blockAdr, indNum));
                 prevBlock = block;
             }
             
             
             Block* chainingBlock = bufferCache->findBlock(chainingBlockAdr);
             if(chainingBlock == NULL)
             {
                 auto iter =insertBufferCacheDataMap.find(chainingBlockAdr);
                 if(iter != insertBufferCacheDataMap.end())
                     chainingBlock =iter->second;
                 else
                 {
                     chainingBlock = new Block();
                     if(false == KVDBServer::getInstance()->diskManager->readBlock(chainingBlockAdr, chainingBlock))
                     {
                         delete chainingBlock;
                         chainingBlock = NULL;
                     }
                     
                     if(chainingBlock != NULL)
                         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(chainingBlockAdr, chainingBlock));
                 }
             }
             
             
             block       = chainingBlock;
             blockAdr    = chainingBlockAdr;
             indNum      = ibaToOffsetIdx(chainingIndBlockAdr, chainingBlockAdr);
             offset      = block->getOffsetByIndNum(indNum);
             data        = block->getData(indNum);
             type        = data->getFormatType();
            
             
         }
         
         if(block != NULL)
             dirtyBlockInfoList.push_back(DirtyBlockInfo(block, false, false, false, blockAdr, indNum));
         
     }
     
     
     // 네임드 캐시 지우기
     if(deleteNamedData != NULL)
         namedCache->deleteData(delComponent, deleteNamedData);
     
     
    // 로그버퍼 -> 블락에서 데이터 지우기 -> 버퍼캐시에서 블락 지우기(슈퍼블럭 1->0, 캐싱할 블럭에서 해당 블럭 정보 지우기)
     for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
     {
         
         Block*     block     = blockInfo.block;
         uint64_t   blockAdr  = blockInfo.blockAddress;
         uint16_t   freeSpace = blockInfo.block->getFreeSpace();
         uint16_t   indNum    = blockInfo.indirectionNum;
         uint16_t   offset    = blockInfo.block->getOffsetByIndNum(indNum);
         Data*      data      = blockInfo.block->getData(indNum);
         
        
         if(blockInfo.isLoging)
         {
             if(data!=NULL)
                 KVDBServer::getInstance()->logBuffer->saveLog(blockInfo.isAllocateBlock,blockInfo.isFreeBlock, blockInfo.isInsert,
                                                           blockAdr, freeSpace, indNum ,offset, data,
                                                           blockInfo.prevBlockAddress, blockInfo.nextBlockAddress);
         }
         
         
         block->deleteData(indNum);
         
         if(blockInfo.isFreeBlock == true)
         {
             auto iter =insertBufferCacheDataMap.find(blockAdr);
             
             if(iter != insertBufferCacheDataMap.end())
                 insertBufferCacheDataMap.erase(iter);
             
             bufferCache->setBitArrayFlag(blockAdr);
             
             bufferCache->deleteDirty(blockAdr);
         }
         
     }
     
     // 캐싱
     caching(parentNamedData);
     
     // 로그파일에 쓴다
     char* log;
     logBuffer->commitLogBuffer(&log);
     logFile->writeLogFile((int)strlen(log), log);
     
     // 디스크에 쓰기
     for(DirtyBlockInfo blockInfo : dirtyBlockInfoList)
     {
         Block*     block    = blockInfo.block;
         uint64_t   blockAdr = blockInfo.blockAddress;
         
         if(blockInfo.isFreeBlock == true)
         {
             char* bitArray = KVDBServer::getInstance()->superBlock->getUsingBlockBitArray();
             KVDBServer::getInstance()->diskManager->writeBitArray(bitArray);
             
             if(block->getIndirectionDataMapSize()==0)
                 continue;
             
         }else
         {
             KVDBServer::getInstance()->diskManager->writeBlock(blockAdr, block);
             block->setDirty(false);
         }
     }
     

     // 로그파일 클리어
     KVDBServer::getInstance()->logFile->clear();
 
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

 uint64_t IOManager::ibaToBa(uint64_t iba)
 {
     uint64_t rootBlockAddress = KVDBServer::getInstance()->superBlock->getRootBlockAddress();
     
     uint64_t distance = (iba - rootBlockAddress) % BLOCK_SIZE;
     uint64_t blockAddress = iba - distance;
 
     return blockAddress;
 }

 uint16_t IOManager::ibaToOffsetIdx(uint64_t iba, uint64_t ba)
 {
     uint8_t  headerSize = Block::getBlockHeaderSize();
     uint16_t offsetIdx = (iba -(ba + headerSize) ) /2;
     
     return offsetIdx;
 }
 
 bool IOManager::compaction(Block* block)
 {
     // 블락 인다이렉션 돌면서 가장 큰 오프셋 순으로 인다이렉션 넘버를 데이터를 반환한다.
     std::vector<uint16_t> offsetList;
     
     int limitOffset = BLOCK_SIZE;
     for(int i = 0; i < block->getIndirectionDataMapSize(); ++i)
     {
         uint16_t offset = block->getLargestOffset(limitOffset);
         offsetList.push_back(offset);
         limitOffset = offset;
     }
     
     std::vector<uint16_t> indirectionNumList;
     for(uint16_t offset : offsetList)
     {
         uint16_t indirectionNum = block->getIndNumByOffset(offset);
        indirectionNumList.push_back(indirectionNum);
     }
     
     // indirectionNumber List를 이용해서 블락에 넣어진 순으로 데이터를 리스트로 가져 온다.
     std::vector<Data*> dataList;
     for(uint16_t indNum : indirectionNumList)
     {
          Data* data = block->getData(indNum);
          dataList.push_back(data);
     }
     
     // 새로 arrangement할 오프셋을 List에 저장 한다.
     offsetList.clear();
     int newOffset = BLOCK_SIZE;
     for(Data* data : dataList)
     {
         newOffset -= data->getDataSize();
         offsetList.push_back(newOffset);
     }
     
     // 블럭을 초기화 시킨다.
     uint64_t chainingBlockAdr = block->getChaingAddress();
     block->init();
     
     // 블럭 세팅한다.
     block->setChainingAddress(chainingBlockAdr);
     
     
     for(int i=0; i < dataList.size(); ++i)
     {
         uint64_t indNum = indirectionNumList[i];
         uint64_t offset = offsetList[i];
         Data* data = dataList[i];
         
         block->insertData(indNum, offset, data);
     }
     return true;
 }

bool IOManager::caching(NamedData* firstParentData)
{
    NamedCache*  namedCache  = KVDBServer::getInstance()->namedCache;
    BufferCache* bufferCache = KVDBServer::getInstance()->bufferCache;
    
    NamedData* parentNamedData = firstParentData;
    
    for(NamedCacheData data: namedCacheDataList)
    {
        NamedData* childNamedData = new NamedData(componentList[data.componentIdx], data.indirectionBlockAdr);
        namedCache->insert(parentNamedData, childNamedData);
        parentNamedData = childNamedData;
    }
    
    // BufferCache 넣기
    for (auto iter = insertBufferCacheDataMap.begin(); iter!= insertBufferCacheDataMap.end(); ++iter)
    {
        if(false == bufferCache->insertBlock2Cache(iter->first, iter->second))
        {
            uint64_t blockAdr = 0;
            Block* block = NULL;
            
            bool result = bufferCache->getDeleteBlock(blockAdr, &block);  // BlockInfo 는 블럭주소 , 블락을 멤버로 갖는 클래스 가상으로 생각해놓은 클래스임
            
            if(result == false)
            {
                bufferCache->insertBlock2Cache(iter->first, iter->second); // 다시 넣어준다.
                continue;
            }
            
            KVDBServer::getInstance()->diskManager->writeBlock(blockAdr, block);
            bufferCache->deleteDirty(blockAdr);
            bufferCache->insertBlock2Cache(iter->first, iter->second);  // 다시 넣어준다.
        }
    }
    
    return true;
}


 IoMgrReturnValue IOManager::checkBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, long lastIdx)
 {

     int        curentIdx       = curIdx;
     uint64_t   blockAddress    = ibaToBa(indirectionBa);
     Block*     block           = NULL;
     
     if(curentIdx < 0)
         curentIdx = 0;
     
     while(true)
     {
         block = KVDBServer::getInstance()->bufferCache->findBlock(blockAddress);
         
         if(block == NULL)
         {
             block = new Block();
             
             if(false == KVDBServer::getInstance()->diskManager->readBlock(blockAddress, block))
             {
                 delete block;
                 
                 IoMgrReturnValue returnVal(NULL, curentIdx, -1);
                 return returnVal; // 블럭이 없다.
             }
             
             insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(blockAddress, block));
             
         }

         std::string comp = componentList[curentIdx];
         
         Data* data = block->getData(comp);
         if(curentIdx < lastIdx)  // 중간 데이터 일때
         {
             if(data == NULL)
             {
                 if(block->getChaingAddress() > 0)  // 블럭체이닝 있을때
                 {
                     blockAddress = block->getChaingAddress();
                     continue;
                 }
                 else   //  중간 데이터인데 현재 블럭에 데이터 없고 체이닝된 블럭도 없을 때
                 {
                     IoMgrReturnValue returnVal(block, curentIdx, -2);
                     return returnVal;
                 }
                 
             }else // 중간데이터인데 값 있을때
             {
                 DirectoryData* dirData = (DirectoryData*)data;
                 
                 uint16_t indNum = block->getIndNumByKey(componentList[curentIdx]);
                 uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAddress, indNum);
                 if(indirectionBa != indBlockAdr)
                 {
                     NamedCacheData namedCacheData(curentIdx, indBlockAdr);
                     namedCacheDataList.push_back(namedCacheData);
                 }
                 
                 blockAddress = ibaToBa(dirData->getIndBlockAddress());
                 curentIdx += 1;
                 continue;
             }
             
         }else // 마지막 데이터 일때
         {
             if(data == NULL)
             {
                 lastBlockChainingAdrList.push_back(blockAddress);
                 if(block->getChaingAddress() > 0)  // 블럭체이닝 있을때
                 {
                     blockAddress    = block->getChaingAddress();
                     continue;
                 }
                 else   // 마지막 데이터인데 현재 블럭에 데이터 없고 체이닝된 블럭도 없을 때
                 {
                     uint16_t indNum = block->getNewIndirectionNumber();
                     uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAddress, indNum);
                     if(indirectionBa != indBlockAdr)
                     {
                         NamedCacheData namedCacheData(curentIdx, indBlockAdr);
                         namedCacheDataList.push_back(namedCacheData);
                     }
                     
                     IoMgrReturnValue returnVal(block, curentIdx, 0);
                     return returnVal;
                 }
             }
             else
             {
                 uint16_t indNum = block->getIndNumByKey(componentList[curentIdx]);
                 uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAddress, indNum);
                 if(indirectionBa != indBlockAdr)
                 {
                     NamedCacheData namedCacheData(curentIdx, indBlockAdr);
                     namedCacheDataList.push_back(namedCacheData);
                 }

                 IoMgrReturnValue returnVal(block, curentIdx, -3);  // 값이 이미 있음
                 return returnVal;
             }
         }
         
     }// while 문 end
     
 }


IoMgrReturnValue IOManager::findBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, long lastIdx)
{
    int        curentIdx       = curIdx;
    uint64_t   blockAddress    = ibaToBa(indirectionBa);
    Block*     block           = NULL;
    
    if(curentIdx < 0)
        curentIdx = 0;
    
    while(true)
    {
        block = KVDBServer::getInstance()->bufferCache->findBlock(blockAddress);
        
        if(block == NULL)
        {
            block = new Block();
            
            if(false == KVDBServer::getInstance()->diskManager->readBlock(blockAddress, block))
            {
                delete block;
                
                IoMgrReturnValue returnVal(NULL, curentIdx, -1);
                return returnVal; // 블럭이 없다.
            }
            
            insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(blockAddress, block));
            
        }
        
        Data* data = block->getData(componentList[curentIdx]);
        
        if(curentIdx < lastIdx)  // 중간 데이터 일때
        {
            if(data == NULL)
            {
                if(block->getChaingAddress() > 0)  // 블럭체이닝 있을때
                {
                    blockAddress = block->getChaingAddress();
                    continue;
                }
                else   //  중간 데이터인데 현재 블럭에 데이터 없고 체이닝된 블럭도 없을 때
                {
                    IoMgrReturnValue returnVal(NULL, curentIdx, -2);
                    return returnVal;
                }
                
            }else // 중간데이터인데 값 있을때
            {
                DirectoryData* dirData = (DirectoryData*)data;
                
                uint16_t indNum = block->getIndNumByKey(componentList[curentIdx]);
                uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAddress, indNum);
                if(indirectionBa != indBlockAdr)
                {
                    NamedCacheData namedCacheData(curentIdx, indBlockAdr);
                    namedCacheDataList.push_back(namedCacheData);
                }
                
                blockAddress = ibaToBa(dirData->getIndBlockAddress());
                curentIdx += 1;
                continue;
            
            }
            
        }else // 마지막 데이터 일때
        {
            lastBlockChainingAdrList.push_back(blockAddress);
            
            if(data == NULL)
            {
                if(block->getChaingAddress() > 0)  // 블럭체이닝 있을때
                {
                    blockAddress    = block->getChaingAddress();
                    continue;
                }
                else   // 마지막 데이터인데 현재 블럭에 데이터 없고 체이닝된 블럭도 없을 때
                {
                    IoMgrReturnValue returnVal(NULL, curentIdx, -3);  // block은 있으나 data를 찾을 수 없었다.
                    return returnVal;
                }
            }
            else
            {
                 uint16_t indNum = block->getIndNumByKey(componentList[curentIdx]);
                 uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAddress, indNum);
                
                if(indirectionBa != indBlockAdr)
                 {
                     NamedCacheData namedCacheData(curentIdx, indBlockAdr);
                     namedCacheDataList.push_back(namedCacheData);
                 }
 
                IoMgrReturnValue returnVal(block, curentIdx, 0);  // 값을 찾았다.
                return returnVal;
            }
        }
        
    }// while 문 end
}









