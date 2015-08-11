#ifndef __K_V_D_B_SERVER_H__
#define __K_V_D_B_SERVER_H__

#include <stddef.h>

class Network;
class WorkerThread;
class XmlData;
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

public:
	Network* network;
	Log* log;
    XmlData* xmlData;
    DiskManager* diskManager;
    LogBuffer* logBuffer;
    LogFile* logFile;
};

#endif //__K_V_D_B_SERVER_H__