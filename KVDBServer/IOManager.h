#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

class InsertRequestInfo;
class InsertDirectoryRequestInfo;
class FindRequestInfo;
class DeleteRequestInfo;

class IOManager
{
public:
	IOManager();
	~IOManager();

public:
    void processInsert(InsertRequestInfo* reqInfo);
    void processInsert(InsertDirectoryRequestInfo* reqInfo);
	void processFind(FindRequestInfo* reqInfo);
	void processDelete(DeleteRequestInfo* reqInfo);

private:

};

#endif // __I_O_MANAGER_H__