#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include <vector>
#include "WorkerThread.h"


class InsertRequestInfo;
class InsertDirectoryRequestInfo;
class FindRequestInfo;
class DeleteRequestInfo;
class Block;


class IOManager : public WorkerThread
{
public:
	IOManager();
	~IOManager();

    ////////////virtual method///////////////
    void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
    
    ////////////////////////////////////////
    void receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void receiveMasterData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    bool parsingQuery(const char* query, int queryLen, RequestInfo** pri);
    
public:
    int8_t processInsert(InsertRequestInfo* reqInfo);
    int8_t processInsert(InsertDirectoryRequestInfo* reqInfo);
	void processFind(FindRequestInfo* reqInfo);
	void processDelete(DeleteRequestInfo* reqInfo);

private:
    std::vector<std::string> split(const std::string &s, char delim);
    
    int8_t checkBufferCacheAndDisk(uint64_t indirectionBa, int curIdx, int lastIdx);
  
    uint64_t ibaToBa(uint64_t iba);
    
};

#endif // __I_O_MANAGER_H__