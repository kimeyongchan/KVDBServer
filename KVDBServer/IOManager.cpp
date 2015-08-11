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
#include <sstream>
#include "Defines.h"
#include "DiskManager.h"
#include "SuperBlock.h"
#include "Network.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
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
   /*
    // ======================================================= 테스트 코드 ==================================================
     // 예상 데이터    Key :  a/b   Value: hello 내지는 그냥  h
     // 디스크에 실제로 써보는거 테스코드
     componentList.clear();
     componentList = split(reqInfo->key, '/');
     
     // 루트블럭 가져오기
     SuperBlock* superBlock = KVDBServer::getInstance()->superBlock;
     uint64_t rootBlockAdr = superBlock->getRootBlockAddress();
     Block* rootBlock = new Block();
     
     // 루트블럭에 넣을 디렉터리를 만든다.
     DirectoryData* data = new DirectoryData();
     data->setFormatType(FLAG_DIRECTORY_DATA);
     data->setKey(componentList[0]);
     
     uint64_t aBlockAdr = rootBlockAdr + 8192;//BLOCK_SIZE;
     Block* aBlock = new Block();
     uint64_t aBlockFirstIndBlockAdr = aBlock->getFirstIndirectionBlockAdr(aBlockAdr);
     
     data->setIndBlockAddress(aBlockFirstIndBlockAdr);
     
     // 루트블럭에 디렉터리 데이터를 넣어준다.
     uint16_t dataSize = data->getDataSize();
     uint16_t newOffset = rootBlock->getNewOffset(dataSize);
     rootBlock->insertData(rootBlock->getNewIndirectionNumber(), newOffset, data);
     
     // a블럭에 실제 b데이터 insert한다.
        // b 키벨류 데이터를 만든다.
     
     KeyValueData* keyValData = new KeyValueData();
     keyValData->setFormatType(FLAG_KEY_VALUE_DATA);
     keyValData->setKey(componentList[1]);
     keyValData->setValue(reqInfo->value);
     
     
        // a 블럭에 정보를 넣어준다.
     uint16_t aBlockNewOffset = aBlock->getNewOffset(keyValData->getDataSize());
     aBlock->insertData(aBlock->getNewIndirectionNumber(), aBlockNewOffset, keyValData);
     
     
     // 더티된 블럭들 써준다.
     KVDBServer::getInstance()->diskManager->writeBlock(rootBlockAdr, rootBlock);
     KVDBServer::getInstance()->diskManager->writeBlock(aBlockAdr, aBlock);
    
  
     delete aBlock;
     delete rootBlock;
     
     DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());
    
    // ======================================================= 테스트 코드 ==================================================
    */
     
 /*
     DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());

     componentList.clear();
     namedCacheDataList.clear();                         // 네임드캐시
     insertBufferCacheDataMap.clear();                   // 버퍼캐시
     std::vector<LogBufferInsertData> logBufferDataList; // 로그 버퍼
     std::map<uint64_t, Block*>  diskWriteDataMap;       // 디스크
     
     NamedData* parentNamedData;
     uint64_t indirectionBlockAdr =0;
     IoMgrReturnValue returnVal(NULL, 0, 0);  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
     
     // 먼저 있는지 확인한다.
     componentList = split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRoodAddr();
     
     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             parentNamedData = nd;  // 네임드 캐시에 넣을때 쓰일 변수
             returnVal = checkBufferCacheAndDisk(nd->getIdtAddr(), i-1, componentList.size()-1);
             
             if(returnVal.returnCode <0)
                 return returnVal.returnCode;
             else
                 break;
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = checkBufferCacheAndDisk(childNd->getIdtAddr(), i, componentList.size()-1);
         indirectionBlockAdr = childNd->getIdtAddr();
         
         if(returnVal.returnCode <0)
             return returnVal.returnCode;
         else
             break;
         
     }// for
     
     
     Block* block = returnVal.block;
     uint64_t blockAdr = ibaToBa(indirectionBlockAdr);
     std::string key = componentList[returnVal.componentIdx];
     
     // 컴팩션
     compaction(block);
     
     
     // 블럭의 프리스페이스와 실제 넣을 데이터 사이즈를 비교해봐야한다.
     // 실제 넣을 데이터 사이즈를 구한다.
     
     uint16_t addingIndirectionByte = 0;
     uint16_t newIndirectionNumber = block->getNewIndirectionNumber() ;
     if(newIndirectionNumber > block->getLastIndirectionNumber()) // 인다이렉션주소를 새로 추가하면 사이즈 2byte 추가 해야 한다.
         addingIndirectionByte += 2;
     
     uint16_t dataSize = sizeof(uint8_t) + key.size() + sizeof(uint32_t) + reqInfo->value.size() + addingIndirectionByte;
     
     if(dataSize <= block->getFreeSpace()) // 블럭에 바로 넣을 수 있다.
     {
         KeyValueData* data = new KeyValueData();
         data->setFormatType(FLAG_KEY_VALUE_DATA);
         data->setKey(key);
         data->setValue(reqInfo->value);
         uint16_t newOffset = block->getNewOffset(data->getDataSize());
         
         // 블럭에 데이터 넣기
         block->insertData(newIndirectionNumber, newOffset, data);
         
         // 로그버퍼에 넣을 데이터
         uint64_t indBlockAdr = block->getIndirectionBlockAdr(blockAdr, newIndirectionNumber);
         LogBufferInsertData logBufData(false, true, indBlockAdr, newOffset, data);
         logBufferDataList.push_back(logBufData);
         
         // 디스크에 쓸 데이터
         diskWriteDataMap.insert(std::pair<uint64_t, Block*>(blockAdr, block));
         
         
     }else // chaining 필요하다.
     {
         uint16_t minimumBlockSize = sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t) + sizeof(uint64_t) + addingIndirectionByte;
         // formatType  +  key length     + key size    +  value length  +  chaining address
         
         if(minimumBlockSize > block->getFreeSpace())
             return -4; // 최소 체이닝 할수 있는 데이터 크기조차 넣을 수 없다.
         
         
         
         uint32_t remainValueSize = reqInfo->value.size();
         Block*   curBlock = block;
         uint64_t logBufferIndBlockAdr = indirectionBlockAdr;
         uint64_t diskBlockAdr = blockAdr;
         
         while(remainValueSize > 0)
         {
             if(curBlock->getFreeSpace() >= (remainValueSize + sizeof(uint8_t) + sizeof(uint8_t) + key.size() + sizeof(uint32_t) + addingIndirectionByte))
             {
                 KeyValueData* data = new KeyValueData();
                 data->setFormatType(FLAG_KEY_VALUE_DATA); // 체이닝 없는 일반 데이터
                 data->setKey(key);
                 
                 std::string curBlockValue =reqInfo->value.substr( reqInfo->value.size()- remainValueSize, remainValueSize);
                 data->setValue(curBlockValue);
                 remainValueSize = 0;
                 
                 uint16_t newOffset = curBlock->getNewOffset(data->getDataSize());
                 curBlock->insertData(curBlock->getNewIndirectionNumber(), newOffset, data);
                 
                 // 로그버퍼에 넣을 데이터
                 uint16_t curBlockNewOffset = curBlock->getNewOffset(data->getDataSize());
                 LogBufferInsertData logBufData(true, true, logBufferIndBlockAdr, curBlockNewOffset, data);
                 logBufferDataList.push_back(logBufData);
                 
                 // 디스크에 쓸 데이터
                 diskWriteDataMap.insert(std::pair<uint64_t, Block*>(diskBlockAdr, curBlock));
                 
                 break;
             }
             
             
             
             
             // 블럭에 넣을 데이터 생성
             KeyValueChainingData* data = new KeyValueChainingData();
             data->setFormatType(FLAG_KEY_VALUE_CHAINING_DATA); // 체이닝 데이터
             data->setKey(key);
             
             if(logBufferDataList.size() !=0)
                 addingIndirectionByte = 2;
             
             int curBlockValueSize = curBlock->getFreeSpace() - (data->getDataSize()+ addingIndirectionByte);
             std::string curBlockValue =reqInfo->value.substr( reqInfo->value.size()-remainValueSize,curBlockValueSize);
             data->setValue(curBlockValue);
             remainValueSize -= curBlockValueSize;
             
             
             uint64_t newChingingBlockAdr = cacheMgr->getNewBlockAdr();  // 캐시로 부터 새로운 주소를 받아온다.
             Block* newChainingBlock = new Block();
             uint64_t newBlockIndAdr = newChainingBlock->getFirstIndirectionBlockAdr(newChingingBlockAdr);
             data->setIndBlockAddress(newBlockIndAdr);
             
             
             // 블럭에 데이터 insert
             uint16_t curBlockNewOffset = curBlock->getNewOffset(data->getDataSize());
             curBlock->insertData(curBlock->getNewIndirectionNumber(), curBlockNewOffset, data); // 이때 freeSpace 줄어든다.
             
             if(remainValueSize >0)
                 curBlock->setChainingAddress(newChingingBlockAdr);
             
             // 버퍼캐시에 넣을 데이터
             insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newChingingBlockAdr, newChainingBlock)); // 체이닝 블럭 넣기
             
             // 로그버퍼에 넣을 데이터
             if(logBufferDataList.size() ==0)
             {
                 LogBufferInsertData logBufData(false, true, logBufferIndBlockAdr, curBlockNewOffset, data);
                 logBufferDataList.push_back(logBufData);
                 
             }else
             {
                 LogBufferInsertData logBufData(true, true, logBufferIndBlockAdr, curBlockNewOffset, data);
                 logBufferDataList.push_back(logBufData);
             }
             
             // 디스크에 쓸 데이터
             diskWriteDataMap.insert(std::pair<uint64_t, Block*>(diskBlockAdr, curBlock));
             
             
             curBlock = newChainingBlock;
             logBufferIndBlockAdr = newBlockIndAdr;
             diskBlockAdr = newChingingBlockAdr;
         }
         
     }
     
     // NamedCache , BufferCache에 넣기  더티 상태
     // 네임드 캐시에 넣기
     for(NamedCacheData data: namedCacheDataList)
     {
         NamedData* childNamedData = new NamedData(componentList[data.componentIdx], data.indirectionBlockAdr);
         namedCache->insert(parentNamedData, childNamedData);
         parentNamedData = childNamedData;
     }
   
     namedCache->commitLruQueue();
     
     
     // BufferCache 넣기
     for (auto iter = insertBufferCacheDataMap.begin(); iter!= insertBufferCacheDataMap.end(); ++iter)
     {
         if(false == bufferCache->insertBlock2Cache(iter->first, iter->second))
         {
             uint64_t blockAdr = 0;
             Block* block = 0;
             
             bool result = bufferCache->getDeleteBlock(blockAdr, block);  // BlockInfo 는 블럭주소 , 블락을 멤버로 갖는 클래스 가상으로 생각해놓은 클래스임
             
             if(dirtyBlockInfo.block == NULL)
             {
                 bufferCache->insertBlock2Cache(iter->first, iter->second) // 다시 넣어준다.
                 continue;
             }
             
             KVDBServer::getInstance()->m_diskManager->writeDisk()(blockAdr, block));
             bufferCache->deleteDirty(blockAdr);
             bufferCache->insertBlock2Cache(iter->first, iter->second)  // 다시 넣어준다.
         }
     }
     
     
     // 로그버퍼에 넣는다
     LogBuffer* logBuffer = KVDBServer::getInstance()->m_logBuffer;
     for(LogBufferInsertData data: logBufferDataList)
         logBuffer->saveLog(data.isAllocateBlock, data.isInsert, data.indBlockAddress, data.offset, data.data);
     
     
     
     // 로그파일에 쓴다
     const char* log = logBuffer->readLogBuffer();
     KVDBServer::getInstance()->m_logFile->writeLogFile(log);
     
     // 로그버퍼 클리어
     logBuffer->clear();
     
     
     //디스크에 쓴다.
     for(auto iter = diskWriteDataMap.begin(); iter != diskWriteDataMap.end(); ++iter)
     {
         uint64_t blockAdr = iter->first;
         Block* block = iter->second;
         
         KVDBServer::getInstance()->m_diskManager->writeDisk()(iter->first, iter->second));
         
         block->setDirty(false);
     }
     
     
     // 로그파일 클리어
     KVDBServer::getInstance()->m_logFile->clear();
     */
     
     return 0;
 }
 
 
 // 새블럭 생성
 int8_t IOManager::processInsert(InsertDirectoryRequestInfo* reqInfo)
 {

     DebugLog("INSERT_DIRECTORY - key : %s", reqInfo->key.c_str());
     
 /*
     componentList.clear();
     insertBufferCacheDataMap.clear();
     namedCacheDataList.clear();
     std::map<uint64_t, Block*>  diskWriteDataMap; //<블럭주소, 블럭>
     
     NamedData* parentNamedData;
     uint64_t indirectionBlockAdr =0;
     IoMgrReturnValue returnVal;  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
     
     
     // 먼저 있는지 확인한다.
     int namedCacheInsertComponentIdx =0; // 네임드캐시 넣을 네임드 데이터 컴퍼넌트 시작 인덱스
     componentList= split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRoodAddr();
     
     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             parentNamedData = nd;  // 네임드 캐시에 넣을때 쓰일 변수
             returnVal = checkBufferCacheAndDisk(nd->getIdtAddr(), i-1, componentList.size()-1);
             
             if(returnVal.returnCode <0)
                 return returnVal.returnCode;
             else
                 break;
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = checkBufferCacheAndDisk(childNd->getIdtAddr(), i, componentList.size()-1);
         indirectionBlockAdr = childNd->getIdtAddr();
         
         if(returnVal.returnCode <0)
             return returnVal.returnCode;
         else
             break;
         
     }// for
     
     
     // 컴팩션한다.
     compaction(returnVal.block);
     
     // 새블럭 만들어서 찾은 마지막 블럭 데이터에 새블럭 첫번째 인다이렉션블럭주소를 저장시켜줘야 한다.
     
     // 1. 새블럭을 만들어준다.
     uint64_t newBlockAdr = cacheMgr->getNewBlockAdr();
     Block* newBlock = new Block();
     uint64_t newBlockFirstIndBa = newBlock->getFirstIndirectionBlockAdr(newBlockAdr);
     
     // 2. 새블럭의 첫번째 인다이렉션 주소를 갖는 디렉토리형 데이터를 생성한다.
     DirectoryData* data = new DirectoryData();
     data->setKey(componentList.back();
      data->setIndBlockAddress(newBlockFirstIndBa);
      
      // 3. 마지막 블럭 소환
      Block* lastDirBlock = returnVal.block;
      uint64_t ba = ibaToBa(indirectionBlockAdr);
      uint16_t offsetIdx =ibaToOffsetIdx(indirectionBa, ba);
      
      // 4. 마지막 블럭 프리사이즈와 넣을 데이터의 크기를 비교한다. -> 비교해서 체이닝까지.... 우선 나중에
      // 5. 체이닝 안되는 데이터 바로 넣어준다.
      uint16_t newOffset = lastDirBlock->getNewOffset(data->getDataSize());
      lastDirBlock->insertData(offsetIdx, newOffset, data);
      
      insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(newBlockAdr, newBlock));
      
      LogBufferInsertData logBufferData    = LogBufferInsertData(false, true, indirectionBlockAdr, newOffset, data);
      LogBufferInsertData logBufferNewData = LogBufferInsertData(true, true, newBlockFirstIndBa, 0, NULL);
      
      diskWriteDataMap.insert(std::pair<uint64_t, Block*>(ba, lastDirBlock));
      diskWriteDataMap.insert(std::pair<uint64_t, Block*>(newBlockAdr, newBlock));
      
      // 캐시에 넣기
      // 네임드 캐시에 넣기
      for(NamedCacheData data: namedCacheDataList)
      {
          NamedData* childNamedData = new NamedData(componentList[data.componentIdx], data.indirectionBlockAdr);
          namedCache->insert(parentNamedData, childNamedData);
          parentNamedData = childNamedData;
      }
   
      namedCache->commitLruQueue();
      
      // 버퍼캐시에 넣기
      for (auto iter = insertBufferCacheDataMap.begin(); iter!= insertBufferCacheDataMap.end(); ++iter)
      {
          if(false == bufferCache->insertBlock2Cache(iter->first, iter->second))
          {
              uint64_t blockAdr = 0;
              Block* block = 0;
              
              bool result = bufferCache->getDeleteBlock(blockAdr, block);  // BlockInfo 는 블럭주소 , 블락을 멤버로 갖는 클래스 가상으로 생각해놓은 클래스임
              
              if(dirtyBlockInfo.block == NULL)
              {
                  bufferCache->insertBlock2Cache(iter->first, iter->second) // 다시 넣어준다.
                  continue;
              }
              
              KVDBServer::getInstance()->m_diskManager->writeDisk()(blockAdr, block));
              bufferCache->deleteDirty(blockAdr);
              bufferCache->insertBlock2Cache(iter->first, iter->second)  // 다시 넣어준다.
          }
      }
      
      
      // 로그버퍼에 넣는다
      LogBuffer* logBuffer = KVDBServer::getInstance()->m_logBuffer;
      logBuffer->saveLog(logBufferData.isAllocateBlock,logBufferData.isInsert, logBufferData.indBlockAddress, logBufferData.offset, logBufferData.data);
      logBuffer->saveLog(logBufferNewData.isAllocateBlock, logBufferNewData.isInsert, logBufferNewData.indBlockAddress, logBufferNewData.offset, logBufferNewData.data);
      
      
      // 로그파일에 쓴다
      const char* log = logBuffer->readLogBuffer();
      KVDBServer::getInstance()->m_logFile->writeLogFile(log);
      
      // 로그버퍼 클리어
      logBuffer->clear();
      
      //디스크에 쓴다.
      for(auto iter = diskWriteDataMap.begin(); iter != diskWriteDataMap.end(); ++iter)
      {
          uint64_t blockAdr = iter->first;
          Block* block = iter->second;
          
          KVDBServer::getInstance()->m_diskManager->writeDisk()(iter->first, iter->second));
          
          block->setDirty(false);
      }
      
      // 로그파일 클리어
      KVDBServer::getInstance()->m_logFile->clear();
   
    */
      return 0;
 }
 
 
 void IOManager::processFind(FindRequestInfo* reqInfo)
 {
     
 //    DebugLog("FIND - key : %s", reqInfo->key.c_str());
  
     /*
    // ======================================================= 테스트 코드 ==================================================
     // 테스트 가져오기 코드   Key : a/b
     componentList.clear();
     componentList = split(reqInfo->key, '/');
     
     // 루트블럭 가져오기
     SuperBlock* superBlock = KVDBServer::getInstance()->superBlock;
     Block* rootBlock = new Block();
     KVDBServer::getInstance()->diskManager->readBlock(superBlock->getRootBlockAddress(), rootBlock);
     
     
     
     // 루트블럭 으로 부터 디렉터리 데이터 가져오기
     Data* data = rootBlock->getData(componentList[0]);
     DirectoryData* dirData =(DirectoryData*)data;
     uint64_t aIndirectionAddress = dirData->getIndBlockAddress();
     
     uint64_t aBlockAdr = ibaToBa(aIndirectionAddress);
     
     Block* aBlock = new Block();
     KVDBServer::getInstance()->diskManager->readBlock(aBlockAdr, aBlock);
     
     Data* findData= aBlock->getData(componentList[1]);
     KeyValueData* keyvalData = (KeyValueData*)findData;
     
     
     delete aBlock;
     delete rootBlock;
     
     DebugLog("FIND - key : %s, value : %s ", keyvalData->getKey().c_str(), keyvalData->getValue().c_str());
     // ======================================================= 테스트 코드 ==================================================
     
    */
     
 /*
     componentList.clear();
     insertBufferCacheDataMap.clear();
     namedCacheDataList.clear();

     NamedData* parentNamedData;
     IoMgrReturnValue returnVal;  // 리턴 블럭이 루트 블럭일때는 인덱스가 -1일것이다 참고 해야 한다.
     
     // 먼저 있는지 확인한다.
     componentList = split(reqInfo->key, '/');
     NamedData* nd = namedCache->getRoodAddr();
     
     for (int i=0; i < componentList.size(); ++i)
     {
         std::string c = componentList[i];
         
         NamedData* childNd = namedCache->findComponent(c, nd); // c: 찾으려는 컴퍼넌트 , nd: 부모 NamedData
         
         if(childNd == NULL)// 없을때
         {
             parentNamedData = nd;
             returnVal = checkBufferCacheAndDisk(nd->getIdtAddr(),i-1, componentList.size()-1);
             
             if(returnVal.block == NULL) // 못찾았을 경우
             {
                 DebugLog("can not find %s", componentList[returnVal.componentIdx]);
                 
             }else  // 찾았을 경우
             {
                 Block* findBlock = returnVal.block;
                 Data* data =findBlock->getData(componentList[returnVal.componentIdx]);
                 int8_t dataType =data->getFormatType();
                 
                 if(dataType == FLAG_DIRECTORY_DATA)
                 {
                     DebugLog("find directory data");
                     
                 }else if(dataType == FLAG_KEY_VALUE_DATA)
                 {
                     DebugLog("find key vlue data");
                     
                 }else if(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
                 {
                     DebugLog("find key vlue chaining data");
                 }
             }
             
             break;
         }
         
         if(i != componentList.size()-1)  // 있을때, 중간 노드 일때
         {
             nd = childNd;
             continue;
         }
         
         // 있을때 맨 마지막일 때
         returnVal = checkBufferCacheAndDisk(childNd->getIdtAddr(),  i, componentList.size()-1);
         if(returnVal.block == NULL) // 못찾았을 경우
         {
             DebugLog("can not find %s", componentList[returnVal.componentIdx]);
             return;
             
         }else  // 찾았을 경우
         {
             Block* findBlock = returnVal.block;
             Data* data =findBlock->getData(componentList[returnVal.componentIdx]);
             int8_t dataType =data->getFormatType();
             
             if(dataType == FLAG_DIRECTORY_DATA)
             {
                 DebugLog("find directory data");
                 
             }else if(dataType == FLAG_KEY_VALUE_DATA)
             {
                 DebugLog("find key vlue data");
                 
             }else if(dataType == FLAG_KEY_VALUE_CHAINING_DATA)
             {
                 DebugLog("find key vlue chaining data");
             }
             break;
         }
     }// for
     
     
     // 캐시에 넣기
     // 네임드 캐시에 넣기
     for(NamedCacheData data: namedCacheDataList)
     {
         NamedData* childNamedData = new NamedData(componentList[data.componentIdx], data.indirectionBlockAdr);
         namedCache->insert(parentNamedData, childNamedData);
         parentNamedData = childNamedData;
     }
     
     namedCache->commitLruQueue();
     
     // 버퍼캐시에 넣기
     for (auto iter = insertBufferCacheDataMap.begin(); iter!= insertBufferCacheDataMap.end(); ++iter)
     {
         if(false == bufferCache->insertBlock2Cache(iter->first, iter->second))
         {
             uint64_t blockAdr = 0;
             Block* block = 0;
             
             bool result = bufferCache->getDeleteBlock(blockAdr, block);  // BlockInfo 는 블럭주소 , 블락을 멤버로 갖는 클래스 가상으로 생각해놓은 클래스임
             
             if(dirtyBlockInfo.block == NULL)
             {
                 bufferCache->insertBlock2Cache(iter->first, iter->second) // 다시 넣어준다.
                 continue;
             }
             
             KVDBServer::getInstance()->m_diskManager->writeDisk()(blockAdr, block));
             bufferCache->deleteDirty(blockAdr);
             bufferCache->insertBlock2Cache(iter->first, iter->second)  // 다시 넣어준다.
         }
     }
      
      */
 
 }


 void IOManager::processDelete(DeleteRequestInfo* reqInfo)
 {
 
     DebugLog("DELETE - key : %s", reqInfo->key.c_str());
 
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
     //uint64_t rootBlockAddress= KVDBServer::getInstance()->cacheMgr->getSuperBlock()->getRootBlockAddress();
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
     return true;
 }
 
/*
 IoMgrReturnValue IOManager::checkBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, int lastIdx)
 {
    uint64_t blockAddress    = ibaToBa(indirectionBa);
     uint16_t indirectionIdx  = ibaToOffsetIdx(indirectionBa, blockAddress);
     
     Block*  block = bufferCache->findBlock(blockAddress);
     
     if(block == NULL)
     {
         block = new Block();
         insertBufferCacheDataMap.insert(std::pair<uint64_t, Block*>(blockAddress, block));
         
         if(false == KVDBServer::getInstance()->m_diskManager->readBlock(blockAddress, diskReadBlock))
         {
             delete block;
             InsertReturnValue returnVal(NULL, curIdx, -1);
             return returnVal; // 블럭이 없다.
         }
     }
     
     //Data* data = block->getData(indirectionIdx);
     Data* data = block->getData(componentList[curIdx]);
     if(curIdx < lastIdx)  // 중간 데이터 일때
     {
         if(data == NULL)
         {
             IoMgrReturnValue returnVal(NULL, curIdx, -2);
             return returnVal;
         }
         else
         {
             DirectoryData* dirData = (DirectoryData*)data;
             
             NamedCacheData namedCacheData(curIdx, indirectionBa);
             namedCacheDataList.push_back(namedCacheData);
             
             return checkBufferCacheAndDisk(dirData->getIndBlockAddress(), curIdx +1, lastIdx);
         }
         
     }else // 마지막 데이터 일때
     {
         if(data == NULL)
         {
             IoMgrReturnValue returnVal(block, curIdx, 0);  // insert 해도 된다.
             return returnVal;
         }
         else
         {
             IoMgrReturnValue returnVal(NULL, curIdx, -3);  // 값이 이미 있음
             return returnVal;
         }
     }
    
     
     IoMgrReturnValue returnVal(NULL, 0, 0);  // 값이 이미 있음
     return returnVal;
 }

*/


