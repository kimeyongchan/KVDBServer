#include "IOManager.h"

#include "RequestInfo.h"
#include "Log.h"

IOManager::IOManager()
{
}


IOManager::~IOManager()
{
}


void IOManager::ProcessInsert(InsertRequestInfo* reqInfo)
{
    
    DebugLog("INSERT - key : %s, value : %s ", reqInfo->key.c_str(), reqInfo->value.c_str());
    
    
    
    
    /*
    
    
    // 먼저 있는지 확인한다.
    map<string, Block> buffCachInsertValMap;
    
    string component = reqInfo->component;
    string[] components = component.split("/"); //    a/b/c 를 /를 구분자로 해서 배열로 갖고 있겠음
    
    NamedData* nd = rootNamedData;
    Block* block;
    
    for( string c : components )
    {
        
        NamedData* childNd = namedCache->findND(c, nd);
        
        if(childNd == NULL)// 없을때
        {   // 하드에서 직접 중간 블럭부터 끝블럭까지 실제로 있는지 확인해야한다.
            
            block = diskIo->find(nd->ba);
            while(block != NULL)
            {
                Data* data = block->find(nextComponent);
                
                if(data == NULL) //data 가  실제 키벨류를 가진 데이터인지 다음 블럭을 가리키는 데이터인지는 몰라도 정보가 없으므로 insert 가능하다.
                    break;
                else
                {
                    if(data->getFormatFlag()== true)
                        block = diskIo->find(data->getValue());
                    else // 실제 keyvalue 데이터가 있다는 뜻이기 때문에 insert 불가능하다
                        return -1;
                    
                }
                
            }
            
            
            
        }else // 끝까지 다 있을때
        {
            if(c == lastComponent) // 최종 BlockAdress 를 이용해 BufferChach에서 찾는다.
            {
                
                
                int64_t ba = ibaToBa(nd->indirectionAdress);
                
                Block* block = bufferCache->findBlock(ba, c);
                
                if(block == NULL)
                    block = bufferCache->findBlockByList(key); // search all
                
                if(block == NULL)
                    block = diskIo->findBlock(ba);
                
                if(block !=NULL) // 이미 있으므로 리턴
                    return -1;
                
            }
            
            
        }
        
        nd = childNd;
        
    }// for문 끝
    
    
    
    
    
    
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
    
}


void IOManager::ProcessInsert(InsertDirectoryRequestInfo* reqInfo)
{
    
    DebugLog("INSERT_DIRECTORY - key : %s", reqInfo->key.c_str());
    
}


void IOManager::ProcessFind(FindRequestInfo* reqInfo)
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


void IOManager::ProcessDelete(DeleteRequestInfo* reqInfo)
{
    
    DebugLog("DELETE - key : %s", reqInfo->key.c_str());
    /*
     block ba key
     
     namedCache->deleteData(ND, component);
     bufferCache->DeleteData(block, component);
     
     compaction
     */
    
}