#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include <vector>

class InsertRequestInfo;
class InsertDirectoryRequestInfo;
class FindRequestInfo;
class DeleteRequestInfo;
class Block;


class IOManager
{
public:
	IOManager();
	~IOManager();

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