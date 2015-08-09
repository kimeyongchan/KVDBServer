#ifndef __SERVER_H__
#define __SERVER_H__

#include <stddef.h>

class Network;
class Log;
class XmlData;
class MasterResponseHandler;
class SlaveResponseHandler;
class ClientResponseHandler;


class Server
{
private:
    Server() {};
	static Server* instance;
    
public:
    static Server* getInstance() {
        if (instance == NULL)
            instance = new Server();
        return instance;
    }

public:
	bool Initialize(int workerThreadCount);
	void Run();
    
private:
	Network* network;
	Log* log;
    XmlData* xmlData;

    MasterResponseHandler* masterResponseHandler;
    SlaveResponseHandler* slaveResponseHandler;
    ClientResponseHandler* clientResponseHandler;
};

#endif //__K_V_D_B_SERVER_H__