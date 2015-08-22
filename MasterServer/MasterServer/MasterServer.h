#ifndef __MASTER_SERVER_H__
#define __MASTER_SERVER_H__

#include <stddef.h>
#include "SlaveInfo.h"

class Network;
class XmlData;
class Log;

class MasterServer
{
private:
    MasterServer() {};
	static MasterServer* m_instance;
    
public:
    static MasterServer* getInstance() {
        if (m_instance == NULL)
            m_instance = new MasterServer();
        return m_instance;
    }

public:
	bool Initialize(int workerThreadCount);
	void Run();

public:
	Network* network;
	Log* log;
    XmlData* xmlData;
    SlaveInfo slaveInfo[2];
};

#endif //__MASTER_SERVER_H__