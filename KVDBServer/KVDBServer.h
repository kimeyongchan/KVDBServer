#ifndef __K_V_D_B_SERVER_H__
#define __K_V_D_B_SERVER_H__

#include <stddef.h>

class Network;
class WorkerThread;
class RequestInfo;
class XmlData;
class DiskManager;
class Log;
class LogBuffer;
class LogFile;
class ClientResponseHandler;
class MasterResponseHandler;

class KVDBServer
{
private:
    KVDBServer() {};
	static KVDBServer* m_instance;
    
public:
    static KVDBServer* getInstance() {
        if (m_instance == NULL)
            m_instance = new KVDBServer();
        return m_instance;
    }

public:
	bool Initialize(int workerThreadCount);
	void Run();
    
//    void SendWorkToWorkerThread(RequestInfo* ri);
//    WorkerThread* GetWorkerThreadArray() { return m_workerThreadArray; }

private:
	Network* network;
	Log* m_log;
    XmlData* xmlData;
    DiskManager* m_diskManager;
    LogBuffer* m_logBuffer;
    LogFile* m_logFile;
    ClientResponseHandler* clientResponseHandler;
    MasterResponseHandler* masterResponseHandler;
};

#endif //__K_V_D_B_SERVER_H__