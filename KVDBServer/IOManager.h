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
    void ProcessInsert(InsertRequestInfo* reqInfo);
    void ProcessInsert(InsertDirectoryRequestInfo* reqInfo);
	void ProcessFind(FindRequestInfo* reqInfo);
	void ProcessDelete(DeleteRequestInfo* reqInfo);

private:

};

#endif // __I_O_MANAGER_H__