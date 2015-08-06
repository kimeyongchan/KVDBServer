#ifndef __K_V_D_B_SERVER_H__
#define __K_V_D_B_SERVER_H__

#include <stddef.h>

class Network;
class WorkerThread;
class RequestInfo;
class DiskManager;
class Log;
class LogBuffer;
class LogFile;

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
    
    void SendWorkToWorkerThread(RequestInfo* ri);
//    WorkerThread* GetWorkerThreadArray() { return m_workerThreadArray; }

private:
	Network* m_network;
	Log* m_log;
	WorkerThread* m_workerThreadArray;
    DiskManager* m_diskManager;
    LogBuffer* m_logBuffer;
    LogFile* m_logFile;
    int m_workerThreadCount;
};

#endif //__K_V_D_B_SERVER_H__